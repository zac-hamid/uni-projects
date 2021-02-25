c = [8 6 10 10 4 9];

A   =  [1 1 1 0 0 0;
        0 0 0 1 1 1];
    
Aeq =  [1 0 0 1 0 0;
        0 1 0 0 1 0;
        0 0 1 0 0 1];
   
% Supply
b = [2000; 2500];

% Demand
beq = [1500; 2000; 1000];

% No upper constraint
x = linprog(c,A,b,Aeq,beq,zeros(6),ones(6)*Inf);

x = reshape(x, [], 2)'