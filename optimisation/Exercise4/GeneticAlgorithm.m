function []=GeneticAlgorithm()

% Ga implementation - 1
gasimple;

% GA implementation -2, real code
% GArealcode;

function [bestsol, bestfun, count]=gasimple(funstr)
clc; clear all; close all; dbstop if error;% program management

global solnew sol pop popnew fitness fitold f range;
if nargin<1,
    % Easom Function with fmax=l at x=pi
    funstr='-cos(x)*exp(-(x-3.1415926)^2)';
end
range=[0 6]; % Range/Domain

% Converting to an inline function
f=vectorize(inline(funstr));

x=linspace(range(1),range(2),101);
z=f(x); figure; plot(x,z,'b'); grid on; hold on;
hdL=plot(0,0,'ro');

% Initializing the parameters
rand('state' ,0'); % Reset the random generator
popsize=20; % Population size
MaxGen=100; % Max number of generations
count=0;    % counter
nsite=2;    % number of mutation sites
pc=0.95;    % Crossover probability
pm=0.05;    % Mutation probability
nsbit=16;   % String length (bits)

% Generating the initial population
popnew=init_gen(popsize,nsbit);
fitness=zeros(1,popsize); % fitness array

% Initialize solution <- initial population
for i=1:popsize,
    solnew(i)=bintodec(popnew(i,:));
end

% Start the evolution loop
for i=1:MaxGen,
    % Record as the history
    fitold=fitness; pop=popnew; sol=solnew;
    for j=1:popsize,
        % Crossover pair
        ii=floor(popsize*rand)+1; jj=floor(popsize*rand)+1;
        % Cross over
        if pc>rand,
            [popnew(ii,:),popnew(jj,:)]=...
            crossover(pop(ii,:),pop(jj,:));
            % Evaluate the new pairs
            count=count+2;
            evolve(ii); evolve(jj);
        end
        % Mutation at n sites
        if pm>rand,
            kk=floor(popsize*rand)+1; count=count+1;
            popnew(kk,:)=mutate(pop(kk,:),nsite);
            evolve(kk);
        end
    end % end for j
    % Record the current best
    bestfun(i)=max(fitness);
    bestsol(i)=mean(sol(bestfun(i)==fitness));
    set(hdL,'xdata',bestsol(i),'ydata',bestfun(i)); pause(0.05);
    title(sprintf('Gen=%03d Chro=%0.3d Fit=%5.3f',i,j,bestfun(i)));
end

% Display results
figure;
subplot (2,1,1); plot(bestsol); title('Best estimates');
subplot(2,1,2); plot(bestfun); title('Fitness');

% All the sub functions

% generation of the initial population
function pop=init_gen(np,nsbit)
% String length=nsbit+l with pop(:,l) for the Sign
pop=rand(np,nsbit+1)>0.5;

% Evolving the new generation
function evolve(j)
global solnew popnew fitness fitold pop sol f;
solnew(j)=bintodec(popnew(j,:));
fitness(j)=f(solnew(j));
if fitness(j)>fitold(j),
    pop(j,:)=popnew(j,:);
    sol(j)=solnew(j);
end

% Convert a binary string into a decimal number
function [dec]=bintodec(bin)
global range;
% Length of the string without sign
nn=length(bin)-1;
num=bin(2:end); % get the binary
% Sign=+1 if bin(1)=0; Sign=-1 if bin(1)=1.
Sign=1-2*bin(1);
dec=0;
% floating point/decimal place in a binary string
dp=floor(log2(max(abs(range))));
for i=1:nn,
    dec=dec+num(i)*2^(dp-i);
end
dec=dec*Sign;

% Crossover operator
function [c,d]=crossover(a,b)
nn=length(a)-1;
% generating a random crossover point
cpoint=floor(nn*rand)+1;
c=[a(1:cpoint) b(cpoint+1:end)];
d=[b(1:cpoint) a(cpoint+1:end)];

% Mutatation operator
function anew=mutate(a,nsite)
nn=length(a); anew=a;
for i=1:nsite,
j=floor(rand*nn)+1;
anew(j)=mod(a(j)+1,2);
end


% demonstrate real-code (number) GA
function []=GArealcode()
clc; clear all; close all; dbstop if error;% program management

% f(x,y)=(4-2.1*x^2+x^4/3)*x^2+x*y+4*(y^2-1)*y^2
[X,Y]=meshgrid(linspace(-3,3,31),linspace(-2,2,31));% grid points
F=(4-2.1*(X.^2)+(X.^4)./3).*(X.^2)+X.*Y+4*(Y.^2-1).*Y.^2;% grided obj function
figure; surf(X,Y,F,'edgecolor','none'); hold on; grid on;% show
xlabel('x'); ylabel('y'); alpha(0.3); view(-30,30);% label

% GA parameters
G=100; N=10; Ff=realmax;% no. of generation, chromosome, min func 
C=[(rand(1,N)-0.5)*6; (rand(1,N)-0.5)*4];% initial chromosome
ix=linspace(0,1-0.5/N,N); Ff_=[];% selection index
for g=1:G,% each generation
  for n=1:N,% each chromosome
    f(g,n)=(4-2.1*(C(1,n).^2)+(C(1,n).^4)./3).*(C(1,n).^2)+...
            C(1,n).*C(2,n)+4*(C(2,n).^2-1).*C(2,n).^2;% obj func
  end;
  if g==1,% first generation
    hdL=plot3(C(1,:),C(2,:),f(g,:),'r.');% show temp results
  else
    set(hdL,'xdata',C(1,:),'ydata',C(2,:),'zdata',f(g,:));% update
  end; drawnow; pause(0.2);
  [mi,iz]=min(f(g,:));% elitisim
  if mi<Ff,% better result
    Ff=mi; FC=C(:,iz);% store
    if g==1,
      hdL2=plot3(FC(1),FC(2),Ff,'b*');
    else
      set(hdL2,'xdata',FC(1),'ydata',FC(2),'zdata',Ff);
    end;
  end;
  Ff_=[Ff_ Ff];
  title(sprintf('Gen=%03d Ff= %5.3f x= %5.3f y= %5.3f',g,Ff, FC(1), FC(2)));% show
  % prepare selection
  f(g,:)=-f(g,:); mi=min(f(g,:)); mx=max(f(g,:));% change to max
  f(g,:)=(f(g,:)-mi)/(mx-mi+eps); f(g,:)=f(g,:)/sum(f(g,:)+eps);% scale
  cf=cumsum(f(g,:));
  for n=1:N,% selection
    z(n)=min(find(cf>=ix(n)));% better fitness?
    Cn(:,n)=C(:,z(n));% copy
  end;
  for n=1:2:N,% crossover
    if rand<0.7,% crossover prob
      r1=rand; r2=rand;% crossover ratio
      C(:,n)=r1*Cn(:,n)+(1-r1)*Cn(:,n+1);% crossover
      C(:,n+1)=r2*Cn(:,n+1)+(1-r2)*Cn(:,n);
    else
      C(:,n)=Cn(:,n); C(:,n+1)=Cn(:,n+1);% copy
    end;
  end;
  for n=1:N,% mutation
    if rand<0.1,% mutation prob
      C(:,n)=[(rand-0.5)*6; (rand-0.5)*4];% mutate, re-generate
    end;
  end;
end;
figure; plot(Ff_); grid on;
xlabel('Generations'); ylabel('Fitness');

