clear all; close all; clc;
syms x;
syms y;
f = (y-x^2)^2 + (1-x)^2;
% f = (x-1)^2 + (y-2)^2;

% Part 2.
grad = gradient(f,[x,y])
hess = hessian(f,[x,y])

hess_inv = inv(hess)

% Part 3.
step = 0.1;
x_ = [-5:step:5];
y_ = [-5:step:5];

[x_,y_] = meshgrid(-5:step:5, -5:step:5);

fsurf(f, 'ShowContours', 'on', 'EdgeColor','none'); alpha 0.5; hold on;
% surfc(x,y,f,'facecolor','none'); hold on;

x0 = [-1, -1]';
% x0 = [-3, -3]';
tau = 0.01;

X(:,1) = x0;
fval(1) = double(subs(f, [x,y], [X(1,1),X(2,1)]));
k = 2;
change = Inf;
while change > tau,
    X(:,k) = X(:,k-1) - double(subs(hess_inv, [x,y], X(:,k-1)')) * double(subs(grad, [x,y], X(:,k-1)'));
    fval(k) = double(subs(f,[x,y], [X(1,k), X(2,k)]));
    change = sum(abs(X(:,k)-X(:,k-1)));
    k = k + 1;
end

plot3(X(1,:),X(2,:), fval, 'ro-');
xlabel('x'); ylabel('y'); zlabel('f');
title(sprintf('x = %.3f, y = %.3f, f = %.3f', X(1,end-1), X(2,end-1), fval(end)));
figure; plot(X(1,:)); grid on; hold on; plot(X(2,:));
xlabel('Iteration');
legend('x','y');


