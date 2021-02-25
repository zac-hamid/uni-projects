% Each particle has it's own value of u chosen randomly from range
% [0.0,0.5]
clear all; close all; clc;
range = [0.0 0.5];
alpha=0.2; beta=0.7;
N = 1000; % number of particles (agents)
G = 10; % number of iterations (generations)

f_best=realmax;
u_best=0;
p = init_pso(N,range); % the particles (the "swarm")
f_best_=[];

for i=1:G,
    
    fprintf('Generation %d\n', i);
    f = 0;
    for j=1:N
        % Generate array of function values corresponding to each particle
        f(j)=obj_func(p(j));
    end
    % Get minimum of this
    [f_min, i_min] = min(f);
    uo = min(p(i_min)); % Find global best (for this generation)
    
    % If this global best beats the current overall best, replace overall
    % best
    if f_min<f_best,
        f_best = f_min;
        u_best = uo;
    end;
    % History tracking
    f_best_=[f_best_ f_best];
    
    % Perform x(t+1) = x(t) + v(t+1) update step
    p = pso_move(p,uo,alpha,beta,range);
    fprintf('Best minimum found so far = %.5f at u = %.5f\n', f_best, u_best);
end

plot(f_best_);


function [un] = init_pso(n,range)
    un = rand(1,n)*(range(2)-range(1)) + range(1);
end

function [p] = pso_move(p,uo,a,b,range)
    p = p.*(1-b)+uo.*b+a.*(rand(1,size(p,2))-0.5);
    p = find_range(p,range);
end

function [p] = find_range(p,range)
    for i=1:length(p),
       if p(i)<=range(1), p(i)=range(1); end
       if p(i)>=range(2), p(i)=range(2); end
    end
end

function [obj] = obj_func(u)
    Tol=0.1; % tolerance
    Tspan=0.72; % integration limit
    X0=[0.09 0.09]; % initial system state
    Options=odeset('RelTol',Tol); % option sets for integration
    [T,X]=ode45(@(t,x) intgrl(t,x,u),[0 Tspan],X0,Options);
    obj=sum(sum(X.^2,2)+0.1*u.^2);
end