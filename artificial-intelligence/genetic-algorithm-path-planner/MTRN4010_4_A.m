% Author: Zachary Hamid, z5059915
% Solution to MTRN4010 Project 4 - Part A

function []=MTRN4010_4_A()

clc; clear all; close all; dbstop if error;
set(0,'defaultaxesfontname','times new roman');

field.range=50; % Range of field the landmarks are in
field.LMs=12;   % Number of landmarks

[fig]=FigureNew(field);
[field]=NewLandmark(field);

GA.N=100; GA.G=2000;            % N = number of chromosomes, G = number of generations
GA.Pc=0.9; GA.Pm=0.3;           % Pc = Crossover probability; Pm = Mutation probability
GA.mind=realmax; GA.mingen=1;   % mind = min. distance found (best sol'n so far), mingen = Generation that mind was found
GA.X=zeros(1,field.LMs);        % Setting chromosomes to all 0
[GA]=NewPopulation(GA,field);   % Initialise a random population

for g=1:GA.G,
    title(sprintf('Gen: %d/%d, Dist: %5.2f at %d', g, GA.G, GA.mind, GA.mingen)); drawnow;
    
    for n=1:GA.N, % Need to find the travel distance as our fitness level
        d(n)=FindDistance(GA.X(n,:),field);
    end;
    [mi,ix]=min(d); % Minimum travel distance
    if mi<GA.mind,  % If an even better solution has been found
        GA.mingen=g;
        GA.mind=mi;
        GA.minX=GA.X(ix,:);
        P=field.LM(:,GA.X(ix,:));
        P=[field.LMstart P field.LMstop]; % To show the current path
        set(field.hdL.path,'xdata',P(1,:),'ydata',P(2,:)); drawnow;
    end;
    % Need to store history of minimum distance for plotting purposes
    GA.mind_(g)=GA.mind;
    % Minimisation problem conversion
    fit=1-d/sum(d);     % Less distance is better so we invert
    fit=fit/sum(fit);   % Fitness is defined with respect to the other chromosomes (the avg of the total fitness)
    cumufit=cumsum(fit);% Better fitting chromosomes will have more copies
    for n=1:GA.N,
        r=rand;
        p=min(find(r<cumufit));
        GA.Y(n,:)=GA.X(p,:);    % Y is temporary storage
    end;
    % Perform crossover
    for n=1:GA.N,
        if rand<GA.Pc,
            p1=ceil(rand*GA.N);
            p2=ceil(rand*GA.N);
            % Randomises how much of GA.Y to flip over
            c=ceil(rand*field.LMs);
            if rand<0.5,
               % Flip over part of p1 and combine with p2
               GA.X(n,:)=[fliplr(GA.Y(p1,1:c)) (GA.Y(p2,c+1:end))];
            else
                % Flip over part of p2 and combine with p1
               GA.X(n,:)=[GA.Y(p1,1:c)   fliplr(GA.Y(p2,c+1:end))];
            end
        end;
    end;
    
    % Perform mutations
    for n=1:GA.N,
        if rand<GA.Pm,
            % Pick a random chromosome
            p=ceil(rand*GA.N);
            % Swap random part of chromosome with another part somewhere
            % else (in the same chromosome)
            r1=ceil(rand*field.LMs);
            r2=ceil(rand*field.LMs);
            tmp=GA.X(p,r1);
            GA.X(p,r1)=GA.X(p,r2);
            GA.X(p,r2)=tmp;
        end;
    end;
    
    % Perform elitism
    
    p=ceil(rand*GA.N); % Pick random chromosome
    GA.X(p,:)=GA.minX; % Set it to the current minimum cost path
    
    % Landmark duplicate removal
    for n=1:GA.N,                           % For each chromosome
        for a=1:field.LMs,                  % For each position in chromosome pattern
            b=find(GA.X(n,:)==a);           % Find all occurrences of a
            if length(b)>1,                 % If there is more than 1 -> Duplicate landmark
                for c=1:field.LMs,          % Again, for each position in chromosome pattern
                    d=find(GA.X(n,:)==c);   % Find which landmark doesn't appear
                    if length(d)==0,        % and replace the duplicate landmark with it
                        GA.X(n,b(1))=c;
                    end;
                end;
            end;
        end;
    end;
end;

figure('unit','normalized',...
    'position', [rand*0.3+0.1 rand*0.2+0.1 0.35 0.35]);
plot(GA.mind_); grid on;
xlabel('Generations'); ylabel('Distance found');


function [fig]=FigureNew(field)
fig=figure('units','normalized','position',[0.1 0.2 0.5 0.5]);
axis([-1 1 -1 1]*field.range); hold on; grid on; axis equal;
xlabel('x-direction'); ylabel('y-direction');

% Generate a new set of landmarks, including start and stop
function [field]=NewLandmark(field)
field.LM=2*(rand(2,field.LMs)-0.5)*field.range;

field.hdL.LM=plot(field.LM(1,:),field.LM(2,:),'b*');
hold on; grid on; axis equal;

field.LMstart=2*(rand(2,1)-0.5)*field.range;
field.LMstop=2*(rand(2,1)-0.5)*field.range;

field.hdL.LMstart=plot(field.LMstart(1),field.LMstart(2), 'ro', 'markerfacecolor', 'r');
text(field.LMstart(1),field.LMstart(2), ' Start','color',[0.8 0 0]);

field.hdL.LMstop=plot(field.LMstop(1),field.LMstop(2), 'go', 'markerfacecolor', 'g');
text(field.LMstop(1),field.LMstop(2),' Stop', 'color', [0 0.3 0]);

field.hdL.path=plot([0 0], [0 0]);

% Randomly generate chromosomes
function [GA]=NewPopulation(GA,field)
for n=1:GA.N,
    GA.X(n,:)=randperm(field.LMs);  % Randomly permutates array of numbers from 1 to field.LMs
end;


% Finds the total distance over all the landmarks from start->stop in the
% order given by the inputted chromosome
function [d]=FindDistance(X,field)
d=0;
for k=1:length(X)-1,
    dx=field.LM(1,X(k+1))-field.LM(1,X(k));
    dy=field.LM(2,X(k+1))-field.LM(2,X(k));
    dxy=sqrt(dx^2+dy^2);
    d=d+dxy;
end;
dx=field.LM(1,X(1))-field.LMstart(1);
dy=field.LM(2,X(1))-field.LMstart(2);
d=d+sqrt(dx^2+dy^2);
dx=field.LM(1,X(end))-field.LMstop(1);
dy=field.LM(2,X(end))-field.LMstop(2);
d=d+sqrt(dx^2+dy^2);
