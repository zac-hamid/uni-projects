close all; clear all; clc;
% Task 1 - Real-code GA

G=100; N=30; Ff=realmax; % Number of generations and agents (chromosomes)
C=[(rand(1,N)*4.8)+0.2;(rand(1,N)*4.8)+0.2;(rand(1,N)*4.8)+0.2]; % Initial chromosome
pC = 0.7; % Crossover probability
pM = 0.1; % Mutation probability
ix=linspace(0,1-0.5/N,N); Ff_ = []; % List of found minimums for plotting at the end
for g=1:G,
    for n=1:N,
        % Calculate the objective function for each agent (chromosome)
        f(g,n) = 20*C(1,n).*C(2,n) + 10*C(2,n).*C(3,n) +...
            20*C(1,n).*C(3,n) + 100/(C(1,n).*C(2,n).*C(3,n));
    end
    [mi,i]=min(f(g,:)); % Elitism step
    if mi<Ff % Is this minimum better than the current best?
        Ff=mi; FC=C(:,i); % If it is, store this chromosome and it's result
    end
    Ff_ = [Ff_ Ff]; % Best fitness history
    
    % Change objective function to make it a maximization problem
    f(g,:) = -f(g,:);
    mi=min(f(g,:));
    mx=max(f(g,:));
    % Scale objective function
    f(g,:)=(f(g,:)-mi)/(mx-mi+eps);
    f(g,:)=f(g,:)/sum(f(g,:)+eps);
    cf=cumsum(f(g,:));
    % Selection step
    for n=1:N
        % Has the chromosome found a better result?
        z(n)=min(find(cf >= ix(n)));
        Cn(:,n)=C(:,z(n));
    end;
    % Crossover step
    for n=1:2:N
        % If generated random number is less than crossover probability
        % Perform crossover
        if rand < pC
            % Randomise crossover ratio (equivalent to taking a random
            % crossover point in binary GA)
            r1=rand; r2=rand;
            % Perform the crossover
            C(:,n) = r1*Cn(:,n) + (1 - r1)*Cn(:,n+1);
            C(:,n+1) = r2*Cn(:,n+1) + (1 - r2)*Cn(:,n);
        else
            % If crossover wasn't performed, copy chromosome
            C(:,n) = Cn(:,n);
            C(:,n+1)=Cn(:,n+1);
        end;
    end;
    
    % Mutation step
    for n=1:N
        if rand < pM
            % If chromosome is to be mutated, regenerate chromosome
            C(:,n)=[(rand*4.8)+0.2; (rand*4.8)+0.2; (rand*4.8)+0.2];
        end;
    end;
end

figure; plot(Ff_)