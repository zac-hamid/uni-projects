function [ x ] = NWC(s,d,m,n )
% n= number of destination (input scalar)
% m= number of plant (input scalar)
% s= vector of supply ( input vector )
% d=vector of demand  (input vector )
% x = occuplied matrix ( output matrix)
% Written by Warut Boonphakdee 
for i=1:m;
    s1(i)=s(i);
end;
for j=1:n;
    d1(j)=d(j);
end;
for j=1:n;
  while d1(j)>0;
    for i=1:m;
       
           if s1(i)>0 &&  d1(j)>0; 
               iall=i;
               jall=j;
               break;
           end;
      end;

    if d1(jall)>s1(iall);
       d1(jall)=d1(jall)-s1(iall);
       x(iall,jall)=s1(iall);
       s1(iall)=0;
    elseif d1(jall)<s1(iall);
        s1(iall)=s1(iall)-d1(jall);
        x(iall,jall)=d1(jall);
        d1(jall)=0;
    elseif d1(jall)==s1(iall); 
         x(iall,jall)=d1(jall);
        d1(jall)=0;
        s1(iall)=0;
       
    end;
   end;
 end;

end

