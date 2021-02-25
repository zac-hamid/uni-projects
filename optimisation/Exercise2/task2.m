close all; clear all; clc;

syms x;
syms y;

% User-defined tolerance
tau = 0.01;

% Golden ratio
R = (3-sqrt(5))/2

% Initial bounds
xL = -5;
xU =  5;
yL = -5;
yU =  5;

rng_x = xU - xL
rng_y = yU - yL

dx  = R*rng_x
dy  = R*rng_y
x2  = xU - dx
x1  = xL + dx

y2 = yU - dy
y1 = yL + dy

% Function
f = (10*(x-2)^2) + (x*y) + (10*(y-1)^2);

f11 = double(subs(f, [x, y], [x1, y1]))
f12 = double(subs(f, [x, y], [x1, y2]))
f21 = double(subs(f, [x, y], [x2, y1]))
f22 = double(subs(f, [x, y], [x2, y2]))

fsurf(f, 'ShowContours', 'on', 'EdgeColor','none'); alpha 0.7; hold on;
plot3(x1,y1,f11,'ro');
plot3(x1,y2,f12,'ro');
plot3(x2,y1,f21,'ro');
plot3(x2,y2,f22,'ro');
err = Inf;

i = 1;

txt = sprintf('i: %d', i)
txt = sprintf('x1: %f, y1: %f, x2: %f, y2: %f', x1, y1, x2, y2)
txt = sprintf('xL: %f, xU: %f, yL: %f, yU: %f', xL, xU, yL, yU)
txt = sprintf('f11: %f, f12: %f, f21: %f, f22: %f', f11, f12, f21, f22)
while err > tau
    i = i + 1;
    m = min([f11,f12,f21,f22])
    % Update interval
    if f11 == m
        fprintf('f11 is min\n');
        xU = x2;
        yU = y2;
        
        x2 = x1;
        y2 = y1;
        
        x1 = xL + R*(xU - xL);
        y1 = yL + R*(yU - yL);
        
    	f11 = double(subs(f, [x, y], [x1, y1]));
        f12 = double(subs(f, [x, y], [x1, y2]));
        f21 = double(subs(f, [x, y], [x2, y1]));
        f22 = f11;
    elseif f12 == m
        fprintf('f12 is min\n');
        xU = x2;
        yL = y1;
        
        x2 = x1;
        y1 = y2;
        
        x1 = xL + R*(xU - xL);
        y2 = yU - R*(yU - yL);
        
        f11 = double(subs(f, [x, y], [x1, y1]));
        f12 = double(subs(f, [x, y], [x1, y2]));
        f21 = f12;
        f22 = double(subs(f, [x, y], [x2, y2]));
    elseif f21 == m
        fprintf('f21 is min\n');
        xL = x1;
        yU = y2;
        
        x1 = x2;
        y2 = y1;
        
        x2 = xU - R*(xU - xL);
        y1 = yL + R*(yU - yL);
        
        f11 = double(subs(f, [x, y], [x1, y1]));
        f12 = f21;
        f21 = double(subs(f, [x, y], [x2, y1]));
        f22 = double(subs(f, [x, y], [x2, y2]));
    elseif f22 == m
        fprintf('f22 is min\n');
        xL = x1;
        yL = y1;
        
        x1 = x2;
        y1 = y2;
        
        x2 = xU - R*(xU - xL);
        y2 = yU - R*(yU - yL);
        
        f11 = f22;
        f12 = double(subs(f, [x, y], [x1, y2]));
        f21 = double(subs(f, [x, y], [x2, y1]));
        f22 = double(subs(f, [x, y], [x2, y2]));
    end
    
    txt = sprintf('i: %d', i)
    txt = sprintf('x1: %f, y1: %f, x2: %f, y2: %f', x1, y1, x2, y2)
    txt = sprintf('xL: %f, xU: %f, yL: %f, yU: %f', xL, xU, yL, yU)
    txt = sprintf('f11: %f, f12: %f, f21: %f, f22: %f', f11, f12, f21, f22)
    
    plot3(x1,y1,f11,'ro');
    plot3(x1,y2,f12,'ro');
    plot3(x2,y1,f21,'ro');
    plot3(x2,y2,f22,'ro');
    
    err = 0.5*((xU - xL) + (yU - yL));
end

fprintf('Final minimum: %f\n', mean([f11,f12,f21,f22]))
title(sprintf('x = %.4f, y = %.4f, f = %.4f', mean([x1,x2]), mean([y1,y2]), mean([f11,f12,f21,f22])));
xlabel('x'); ylabel('y'); zlabel('f');

% Q3
% First derivatives
df_x = diff(f,x);
df_y = diff(f,y);

% Second derivatives
df_xx = diff(df_x,x);
df_xy = diff(df_x,y);
df_yx = diff(df_y,x);
df_yy = diff(df_y,y);

% Hessian matrix
% [20,  1]
% [ 1, 20]
% Using principal minors method for determining if matrix is positive
% definite:
% A_0 = 20 > 0
% A_1 = (20*20) - (1*1) = 399 > 0
% Therefore Hessian matrix is positive definite
H = [df_xx, df_xy;
    df_yx, df_yy];
