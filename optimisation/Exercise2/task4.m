clear all; close all; clc;
syms lambda;

f = (0.75/(1+lambda^2)) + 0.65*lambda*atan(1/lambda) - 0.65;

% Part 1 - Bracketing method

% Step 1. Pick 3 arbitrary points close to 0.01 (as suggested by exercise
% document)

offset = 0.1;

lambda_1 = 0.11;
lambda_0 = lambda_1 - offset;
lambda_2 = lambda_1 + offset;

fprintf('Bracketing: \n');
% Increasing offset multiplier based on number of iterations allows us to
% find a containing bracket faster at the cost of the bracket being larger
i = 1;
while 1,
    f0 = double(subs(f, lambda, lambda_0));
    f1 = double(subs(f, lambda, lambda_1));
    f2 = double(subs(f, lambda, lambda_2));
    
    if f0 < f1
       if f2 < f1
          % Finished
          break;
       else
           % f0 < f1 < f2 -> need a point 3 > 2
           lambda_0 = lambda_1;
           lambda_1 = lambda_2;
           lambda_2 = lambda_2 + offset;
       end
    else
        % f1 < f0
        if f2 > f1
           % found a minimum
           lambda_0 = lambda_0 + offset;
           lambda_2 = lambda_2 + offset;
        else
           % f2 < f1 < f0 -> need a point 3 < 0
           lambda_2 = lambda_1;
           lambda_1 = lambda_0;
           lambda_0 = lambda_0 - offset;
        end
    end
    offset = offset * 2;
    i = i + 1;
end
disp([i lambda_0 lambda_2])

fplot(f, [lambda_0, lambda_2]); hold on;
xlabel('\lambda'); ylabel('f');

tau = 0.01;

% 2. Fibonacci Method
fprintf('Fibonacci Method: \n');
x_L = lambda_0; x_H = lambda_2;
F(1) = 0; F(2) = 1;
while 1 / F(end) > tau,
  F(end + 1) = F(end) + F(end - 1);
end;

F(1:2)=[];
N = size(F, 2) - 1;
for k = 1:N,
  rho(k) = 1 - F(N-k+1) / F(N-k+2);
end;

T = 1;
rho(end) = rho(end) - tau;
x_1 = x_L + rho(T) * (x_H - x_L);
x_2 = x_H - rho(T) * (x_H - x_L);
f_1 = double(subs(f, lambda, x_1));
f_2 = double(subs(f, lambda, x_2));
plot([x_1, x_2], [f_1, f_2],'*');
while T < N,
  if f_1 > f_2,
    f_2 = f_1;
    x_H = x_2;
    x_2 = x_1;
    x_1 = x_L + rho(T + 1) * (x_H - x_L);
    f_1 = double(subs(f, lambda, x_1));
  elseif f_2 < f_1,
    f_1 = f_2;
    x_L = x_1;
    x_1 = x_2;
    x_2 = x_H - rho(T + 1) * (x_H - x_L);
    f_2 = double(subs(f, lambda, x_2));
  else
    break;
  end;
  T = T + 1;
  plot([x_1 x_2],[f_1 f_2],'*');
end;
x_mean = mean([x_1 x_2]);
f_mean = mean([f_1 f_2]);
disp([T x_mean f_mean]);
title(sprintf('Fibonacci \\lambda = %.4f, f = %.4f', x_mean, f_mean));


% 3. Bisection Method
figure;
fplot(f, [lambda_0, lambda_2]); hold on;
xlabel('\lambda'); ylabel('f');

fprintf('Bisection Method: \n');
dfunc = diff(f, lambda);
x_L = lambda_0;
x_H = lambda_2;
x = 0.5 * (x_L + x_H);
f_val = double(subs(f, lambda, x));
df_val = double(subs(dfunc, lambda, x));
T = 1;
plot(x, f_val, '*');
while abs(df_val) > tau,
  if df_val > 0,
    x_L = x;
  elseif df_val < 0,
    x_H = x;
  end;
  x = 0.5 * (x_L + x_H);
  f_val = double(subs(f, lambda, x)); 
  df_val = double(subs(dfunc, lambda, x));
  T = T + 1;
  plot(x, f_val, '*');
end;
disp([T x f_val]);

title(sprintf('Bisection \\lambda = %.4f, f = %.4f', x, f_val));