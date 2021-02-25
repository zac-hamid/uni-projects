clear all; close all; clc;

% Number of towns
T = 50;

G = 50; % Number of generations
N = 200; % Number of ants

% Define region bounds, for simplicity only allow square region so
% xbounds = ybounds
region_min = 0;
region_max = 100;

towns = randi([region_min region_max],T,2) % Randomly generate a list of T towns with x and y coordinates in the region [region_min, region_max] = [0, 100]

% Calculate pairwise distances between every town
for i=1:T,
    for j=1:T,
        d(i,j) = sqrt((towns(j,1)-towns(i,1))^2 + (towns(j,2)-towns(i,2))^2);
        if i~=j,
            inv_d(i,j) = 1/(d(i,j)+eps);
        else
            inv_d(i,j) = 0;
        end
    end
end

% Normalize inverse distance matrix
inv_d = inv_d/sum(inv_d(:));

% Plot the towns
figure;
s = string([1:T]);
scatter(towns(:,1),towns(:,2), 'k.'); grid on; text(towns(:,1)+1, towns(:,2)+1,s);
hold on;
for t=1:T,
   S(t)=towns(t,1)^2+towns(t,2)^2; 
end

[~,i]=min(S);
start.town=towns(i,:);
start.i = i;
[~,i]=max(S);
final.town=towns(i,:);
final.i = i;
plot(start.town(1),start.town(2),'go');
plot(final.town(1),final.town(2),'ro');
alpha=0.995;
beta=(1-alpha);
gamma=0.95;
pheremone = ones(T,T)/(T^2); % Initial pheremone matrix is uniformly distributed over a matrix of TxT

best_travel = [];
best_travel_dist = realmax;

hdl_best = plot(0,0,'r-');
hdl_best_current_gen = plot(0,0,'b-');

for i=1:G
    % Set travel=0, visit=0 (in a matrix)
    total_travel_dist = 0;
    visit = zeros(T,T);
    % For each ant
    for j=1:N
        track = []; % List of visited towns for this ant
        travel_dist = 0; % Travel distance for this ant
        % Start ant at a random position in the region
        start_pos = rand(1,2)*(region_max-region_min) + region_min;
        % Get the two closest towns to this position
        start_pos = repmat(start_pos,[T,1]);
        % Sum difference along dim 2 (since towns is Tx2), element-wise square and then sqrt
        % To get distance from ant start position to all towns
        town_dists = sqrt(sum((towns-start_pos).^2,2));
        % Sort from lowest distance to highest
        [~,k]=sort(town_dists,'ascend');
        track = k(1:2); % Set closest 2 towns to visited
        travel_dist = d(k(1),k(2)); % Add the distance between these two towns to travel distance
                                    % since we assume that the ant has
                                    % traveled between the two towns
                                    % already
        n = 0; % Used for loop detection
        valid_track = true;
        % While the ant hasn't traveled to the starting or final towns
        while(track(end) ~= start.i && track(end) ~= final.i)
            if n > T-3 % Once > T towns have been visited, must be stuck in a loop, therefore invalidate track
                valid_track = false;
                break;
            end
            
            % Probability of travel
            p = (pheremone(track(end),:).^alpha).*(inv_d(track(end),:).^beta);
            p(track)=0; % Set probability of travel towards already visited towns to 0
            [~,m] = max(p); % Get the maximum probability
            track(end+1) = m(1); % Visit the town that gave the maximum probability of travel
            travel_dist(end+1) = d(track(end-1),track(end)); % Add travel distance to array
            n = n+1;
        end
        
        if valid_track
            set(hdl_best_current_gen,'xdata',towns(track,1),'ydata',towns(track,2));
            title(sprintf('Generation=%d Ant=%d',i,j));
            drawnow;
            % Lay down pheremone
            for l=1:length(track)-1
                visit(track(l),track(l+1))=visit(track(l),track(l+1))+1;
            end
            % Normalize visit matrix
            visit=visit/max(travel_dist);
        end
    end
    pheremone = pheremone + visit;
    pheremone = pheremone / sum(pheremone(:));
    pheremone = pheremone.*gamma;
    set(hdl_best_current_gen,'xdata',[],'ydata',[]);
    track = start.i;
    
    while track ~= final.i
        p = (pheremone(track(end),:).^alpha).*(inv_d(track(end),:).^beta);
        p(track)=0; % Set probability of travel towards already visited towns to 0
        [~,m] = max(p); % Get the maximum probability
        track(end+1) = m(1); % Visit the town that gave the maximum probability of travel
        total_travel_dist = total_travel_dist + d(track(end-1),track(end)); % Add travel distance to total travel distance
    end
    
    if total_travel_dist < best_travel_dist
       best_travel_dist = total_travel_dist;
       set(hdl_best, 'xdata', towns(track,1), 'ydata', towns(track,2));
    end
    
    best_travel = [best_travel best_travel_dist];
    fprintf('Generation = %d Travel distance = %.4f\n', i, best_travel_dist);
end
figure; plot(best_travel); grid on;
xlabel('Generations');
ylabel('Travel distance');