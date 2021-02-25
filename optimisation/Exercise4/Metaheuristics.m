function []=Metaheuristics()
% Metaheuristics

dbstop if error;
clc; clear all; close all; 

% BubbleSort;
% QuickSort;
% RandomWalk1D;
% RandomWalk2D;
% MarkovChain;
SimulatedAnnealing;

% Bubble Sort
function []=BubbleSort()
clc; clear all; close all; 
A=10; A=randperm(A);
N=length(A); disp([0 0 A]);
for j=1:N-1,
  for i=1:N-1,
    if A(i)>A(i+1),
      tmp=A(i); A(i)=A(i+1); A(i+1)=tmp;
    end;
    disp([i j A]);
  end;
end;

% Quick Sort
function []=QuickSort()
clc; clear all; close all; 
A=10; A=randperm(A); disp(A);
B=QuickSort_(A); disp(B);

function [A]=QuickSort_(A)
if numel(A)<=1,
  return;
end;
pivot=A(1);
B=[A(A<pivot) A(A==pivot) A(A>pivot)]; disp([pivot B]);
A=[QuickSort_(A(A<pivot)) A(A==pivot) QuickSort_(A(A>pivot))];

% Random walk 1D
function []=RandomWalk1D()
clc; clear all; close all; 
figure;
x=rand(1,2); hdL=stairs(1:2,x); grid on; hold on;
xlabel('Steps'); ylabel('Walk');
T=500; p=0.5;
for t=size(x,2)+1:T,
  r=rand;
  if r<p,
  x(t)=x(t-1)+1;
  elseif r>p,
    x(t)=x(t-1)-1;
  else
    x(t)=x(t-1);
  end;
  set(hdL,'xdata',1:t,'ydata',x);
  title(sprintf('var=%5.3f',var(x)));
  pause(0.01);
end;

% Random walk 2D
function []=RandomWalk2D()
clc; clear all; close all; 
figure;
x=rand(2,2); hdL1=stairs(x(1,:),x(2,:)); 
grid on; hold on; axis equal;
hdL2=plot(x(1,end),x(2,end),'ro','markerfacecolor','r');
xlabel('Steps-X'); ylabel('Steps-Y');
T=500; for t=size(x,2)+1:T,
  r=rand(2,1)-0.5;
  x(:,t)=x(:,t-1)+r;
  set(hdL1,'xdata',x(1,:),'ydata',x(2,:));
  set(hdL2,'xdata',x(1,end),'ydata',x(2,end));
  title(sprintf('norm(cov)=%5.3f',norm(cov(x))));
  pause(0.02);
end;

% Markov Chain
function []=MarkovChain()
clc; clear all; close all; 
P=5; P=rand(P,P); sumP=sum(P,2);
for p=1:size(P,1),
  P(p,:)=P(p,:)/sumP(p);
end;
figure; hdL=bar3(P); ax=axis; axis tight;
T=10; for t=1:T,
  P_=P^t;
  bar3(P_); axis(ax); alpha 0.5;
  title(sprintf('t=%d',t));
  pause(0.5);
end;

% Simulated Annealing
function []=SimulatedAnnealing()
clc; clear all; close all; dbstop if error;

% Rosenbrock's function with f*=0 at (1,1)
fstr='(1-x)^2+100*(y-x^2)^2';% obj function
f=vectorize(inline(fstr));% Convert into an inline function
R=3; R_=linspace(-R,R,20);% range of decision variables
[x,y]=meshgrid(R_,R_);% generate grids
surfc(x,y,f(x,y),'edgecolor','none');% show the function landscape
xlabel('x'); ylabel('y'); zlabel('f','rotation',0);
hold on; grid on; alpha 0.5;

% Initializing parameters and settings
T_init =1.0; % Initial temperature
T_min = 1e-10; % Final stopping temperature
F_min = -1e+100; % Min value of the function
max_rej=1000; % Maximum number of rejections
max_run=500; % Maximum number of runs
max_accept = 15; % Maximum number of accept
k = 1; % Boltzmann constant
apa=0.95; % Cooling factor
Enorm=1e-8; % Energy norm
guess=randn(1,2); ns=guess; % Initial guess

% Initializing the counters i,j etc
i= 0; j = 0; accept = 0; totaleval = 0;

% Initializing various values
T = T_init;
E_init = f(guess(1),guess(2));
E_old = E_init; E_new=E_old;
best=guess; % initially guessed values
hdL=plot3(guess(1),guess(2),E_new,'yo',...
  'markersize',5,'linewidth',2,'markerfacecolor','y');
hdLb=plot3(guess(1),guess(2),E_new,'ro',...
  'markersize',5,'linewidth',2); view(-35,60); drawnow;

% Starting the simulated annealling
while ((T > T_min) & (j <= max_rej) & E_new>F_min)
  i = i+1;
  % Check if max numbers of run/accept are met
  if (i >= max_run) | (accept >= max_accept)
    T = apa*T;% Cooling according to a cooling schedule
    totaleval = totaleval + i;
    i = 1; accept = 1; % reset the counters
  end;
  % Function evaluations at new locations
  ns=ns+rand(1,2)*randn;
  while (any(abs(ns)>R)),
    ns=guess+rand(1,2)*randn;
  end;
  E_new = f(ns(1),ns(2));
  set(hdL,'xdata',ns(1),'ydata',ns(2),'zdata',E_new);
  % Decide to accept the new solution
  DeltaE=E_new-E_old;
  if (-DeltaE > Enorm), % Accept if improved
    best = ns; E_old = E_new;
    accept=accept+1; j = 0;
  end;
  % Accept with a small probability if not improved
  if (DeltaE<=Enorm & exp(-DeltaE/(k*T))>rand );
    best = ns; E_old = E_new;
    accept=accept+1;
  else
    j=j+1;
  end;
  % Update the estimated optimal solution
  f_opt=E_old;
  title(sprintf('f=%5.3f X= %5.3f %5.3f',f_opt,best(1),best(2)));
  set(hdLb,'xdata',best(1),'ydata',best(2),'zdata',f_opt); drawnow;
end;
% Display the final results
disp(strcat('Obj function :',fstr));
disp(strcat('Evaluations :', num2str(totaleval)));
disp(strcat('Best solution:', num2str(best)));
disp(strcat('Best objective:', num2str(f_opt)));
