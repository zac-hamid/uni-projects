
function dx = intgrl(t,x,u)
dx=zeros(2,1);
dx(1)=-(2+u)*(x(1)+0.25)+(x(2)+0.5)*exp(25*x(1)/(x(1)+2));
dx(2)=0.5-x(2)-(x(2)+0.5)*exp(25*x(1)/(x(1)+2));
end