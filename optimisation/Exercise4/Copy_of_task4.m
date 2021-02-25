clear all; close all; clc;

% Number of towns
T = 50;
H = 6; % number of hubs

G = 100; % Number of generations
C = 20; % Number of chromosomes
pC = 0.7; % Cross-over probability
pM = 0.1; % Mutation probability

% Define region bounds, for simplicity only allow square region so
% xbounds = ybounds
region_min = 0;
region_max = 100;
global towns;
towns = randi([region_min region_max],T,2) % Randomly generate a list of T towns with x and y coordinates in the region [region_min, region_max] = [0, 100]

% Calculate pairwise distances between every town
for i=1:T,
    for j=1:T,
        d(i,j) = sqrt((towns(j,1)-towns(i,1))^2 + (towns(j,2)-towns(i,2))^2);
    end
end

% Each chromosome needs H 1's and the rest 0's, needs to be a length of T
% bits

chromosomes = zeros(C,T);
for i=1:C,
    % This chooses H random integers WITHOUT replacement from integers
    % 1 to T (that is, it chooses H UNIQUE random towns) and sets the
    % corresponding town to 1 (to a hub)
    chromosomes(i,randsample(T,H)) = 1;
end

next_chromosomes = chromosomes;

best_cost = realmax;
best_chromosome = [];

best_cost_history = [];
% For each generation
for g=1:G,
    chromosomes = next_chromosomes;
    % For each chromosome
    for c=1:C,
        f(c) = fitness(chromosomes(c,:),d);
        %        fprintf("%.4f\n",f);
    end
    F = f/sum(f); % Normalise fitness values
    [~,ix] = min(F);
    if F(ix) < best_cost,
        % Store solution
        best_cost = F(ix);
        best_chromosome = chromosomes(ix,:);
    end
    best_cost_history = [best_cost_history; best_cost];
    % Choose best solution, goes to next generation, others get
    % regenerated by crossover/mutation
    next_chromosomes(ix,:) = chromosomes(ix,:);
    for c=1:2:C,
        if c == ix || c + 1 == ix, continue; end
        if rand<=pC,
            %             fprintf('CROSSOVER!\n');
            % Select two random chromosomes
            % Perform crossover
            nn = T-1;
            c_point = floor(nn*rand)+1;
            next_chromosomes(c,:) = [chromosomes(c,1:c_point) chromosomes(c+1,c_point+1:end)];
            next_chromosomes(c+1,:) = [chromosomes(c+1,1:c_point) chromosomes(c,c_point+1:end)];
        else
            % Just copy chromosome through to next generation UNLESS it
            % gets mutated
            next_chromosomes(c,:) = chromosomes(c,:);
            next_chromosomes(c+1,:) = chromosomes(c+1,:);
        end
    end
    for c=1:C,
        if c == ix, continue; end
        %        if rand<=pM || sum(next_chromosomes(c,:)) ~= H,
        % %            fprintf('MUTATE!\n');
        %            % Select a random crossover
        %            % Mutate chromosome
        %            next_chromosomes(c,:) = zeros(1,T);
        %            next_chromosomes(c,randsample(T,H)) = 1;
        %        end
        
        if sum(next_chromosomes(c,:)) ~= H,
            % If the chromosome contains too many or too few hubs, randomly
            % remove or add hubs
            while sum(next_chromosomes(c,:)) ~= H,
                % Chromosome has too many hubs
                if sum(next_chromosomes(c,:)) > H
                    % Randomly remove hubs so that chromosome has H hubs
                    y=find(next_chromosomes(c,:));
                    next_chromosomes(c,randsample(y,sum(next_chromosomes(c,:))-H)) = 0;
                else
                    % Chromosome has too few hubs
                    x=find(next_chromosomes(c,:)==0);
                    % Randomly add hubs so that chromosome has H hubs
                    next_chromosomes(c,randsample(x,H-sum(next_chromosomes(c,:)))) = 1;
                end
            end
        end
        if rand<=pM
            % Randomly switch 1 hub to another town
            x=find(next_chromosomes(c,:)==0);
            y=find(next_chromosomes(c,:));
            next_chromosomes(c,randsample(x,1)) = 1;
            next_chromosomes(c,randsample(y,1)) = 0;
        end
    end
end

figure;
plot(best_cost_history); xlabel('Generations'); ylabel('Fitness value');
figure;

scatter(towns(:,1),towns(:,2), 'k.'); hold on; grid on;
text(towns(:,1)+1,towns(:,2)+1,string(1:size(towns,1)));
% m = chromosomes(randi(C),:);
m = best_chromosome;
x = find(m);
hdl_hub = scatter(towns(x,1),towns(x,2), 'ro', 'MarkerFaceColor','r');

% connect towns to their nearest hub and plot
y = find(m==0);
for i=1:length(y)
    o = [];
    for j=1:length(x)
        o = [o; [x(j), d(y(i),x(j))]];
    end
    o = sortrows(o,2);
    plot(towns([y(i), o(1,1)],1),towns([y(i), o(1,1)],2), 'b-');
end

x_avg = mean(towns(x,1));
y_avg = mean(towns(x,2));
h_list = [x' atan2(towns(x,2)-y_avg,towns(x,1)-x_avg)];
h_list = sortrows(h_list,2);

for i=1:size(h_list,1)-1
   plot(towns([h_list(i,1),h_list(i+1,1)],1),towns([h_list(i,1),h_list(i+1,1)],2), 'g-', 'MarkerSize', 20);
%    fprintf('Connecting town %d to town %d\n',h_list(i,1),h_list(i+1,1));
end
plot(towns([h_list(size(h_list,1),1),h_list(1,1)],1),towns([h_list(size(h_list,1),1),h_list(1,1)],2), 'g-', 'MarkerSize', 20);
% fprintf('Connecting town %d to town %d\n',h_list(size(h_list,1),1),h_list(1,1));

% d = distance matrix between all towns
% c = chromosome
function [fit] = fitness(c,d)
% To determine fitness, need to determine distance between hubs and
% distance from towns to hubs

global towns;

% towns will connect to the closest hubs

% Distance between hubs is $1 for each unit travelled

% Get all hubs in chromosome c
x = find(c);
dist = 0;
x_avg = mean(towns(x,1));
y_avg = mean(towns(x,2));
% Get theta component of polar coordinates using avg hub point as origin for hubs to order them
h_list = [x' atan2((towns(x,2)-y_avg),(towns(x,1)-x_avg))];
h_list = sortrows(h_list,2);
for i=1:size(h_list,1)-1
    dist = dist + d(h_list(i,1),h_list(i+1,1));
end
dist = dist + d(h_list(size(h_list,1),1), h_list(1,1));

% Calculate all unique pairwise distances between hubs to add to total
% fitness function
% for i=1:length(x)-1
%     y = x((i+1):end);
%     for j=1:length(y)
%         dist = dist + d(x(i),y(j));
%         %            fprintf("Distance between T=%d (%d,%d), and T=%d (%d,%d) = %.3f\n", x(i),towns(x(i),1), towns(x(i),2), y(j),towns(y(j),1),towns(y(j),2), d(x(i),y(j)));
%     end
% end

% for each town that is NOT a hub find
% closest hub and calculate distance
% add this distance to the total distance * 2
z = find(c==0);
for i=1:length(z),
    for j=1:length(x)
        dist = dist + 2*d(z(i),x(j));
        %           fprintf("Distance between T=%d (%d,%d), and T=%d (%d,%d) = %.3f\n", z(i),towns(z(i),1), towns(z(i),2), x(j),towns(x(j),1),towns(x(j),2), d(z(i),x(j)));
    end
end
fit=dist;
end