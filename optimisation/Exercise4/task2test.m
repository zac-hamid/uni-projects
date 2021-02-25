
for i=0:0.01:0.5,
   obj_func(i)
end


function [obj] = obj_func(u)
    Tol=0.1; % tolerance
    Tspan=0.72; % integration limit
    X0=[0.09 0.09]; % initial system state
    Options=odeset('RelTol',Tol); % option sets for integration
    [T,X]=ode45(@(t,x) intgrl(t,x,u),[0 Tspan],X0,Options);
    obj=sum(sum(X.^2,2)+0.1*u.^2);
end