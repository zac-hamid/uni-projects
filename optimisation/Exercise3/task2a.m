
clear all; close all; clc;
syms x y lambda;

f = x*y*(2+x);
g = x^2 + y^2 - 2;

tau = 0.15;

% Plot objective function and constraint
[x_grid, y_grid] = meshgrid(-5:0.1:5,-5:0.1:5);
f_grid = double(subs(f, {x,y}, {x_grid, y_grid}));
surf(x_grid, y_grid, f_grid, 'LineStyle', 'none'); hold on;
xlabel('x'); ylabel('y'); zlabel('f');
constrain = (abs(x_grid.^2 + y_grid.^2 - 2) <= tau);
scatter3(x_grid(constrain), y_grid(constrain), f_grid(constrain), 'filled')

% Set up Lagrangian
L = f + lambda*g

% Get partial derivatives of Lagrangian
J = jacobian(L, [x, y, lambda])

% Ensure we only get real solutions from solve()
assume(x,'real'); assume(y, 'real'); assume(lambda, 'real');

S = solve(J(1),J(2),J(3));

% Substitute solutions for x and y into the objective function
a = double(subs(f, {x,y}, {S.x, S.y}))

% Get minimum and maximum
[f_min, i] = min(a)
[f_max, j] = max(a)

% Get corresponding min and max for x and y values
x_min = S.x(i);
y_min = S.y(i);
x_max = S.x(j);
y_max = S.y(j);

% Plot extrema
for k=1:size(a),
    fprintf("a[%d]: (%.3f, %.3f) = %.3f\n", k, S.x(k), S.y(k), a(k));
    scatter3(S.x(k),S.y(k),a(k)+2, 'wo', 'filled');
    s = sprintf("(%.3f, %.3f) = %.3f", S.x(k), S.y(k), a(k));
    if k==i,
        text(S.x(k),S.y(k),a(k)+30,"Minimum: " + s);
    elseif k==j,
        text(S.x(k),S.y(k),a(k)+40,"Maximum: " + s);
    else
        text(S.x(k),S.y(k),a(k)+40, s, 'HorizontalAlignment', 'right');
    end
end
fprintf("min point [%f, %f], f = %f\n", x_min, y_min, f_min)
fprintf("max point [%f, %f], f = %f\n", x_max, y_max, f_max)
