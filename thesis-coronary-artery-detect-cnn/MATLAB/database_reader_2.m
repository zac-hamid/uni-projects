databases = ["staffiii";"ptbdb"];
selected_database = 2;

[data,status] = urlread(char(strcat("http://physionet.org/physiobank/database/", databases(selected_database),"/CONTROLS")));
if (status ~= 1)
    disp("Error: Couldn't connect to PhysioNet Database!");
end

records = regexp(data,'\n','split');

records_trim = records;
if (any(contains(records,'/')))
    records_trim = extractAfter(records,'/');
end

disp(size(records));

%(size(records,2)-1)
for i=65:81
    % Get the signals
    [ecg, Fs, tm] = rdsamp(char(strcat(databases(selected_database), '/', records(i))), [], 100000);
    %for staffiii database it's a 12-lead ECG with signal channels V1-V6, I,
    %II, III
    %AVR, AVL and AVF must be calculated
    %https://pdfs.semanticscholar.org/8160/8b62b6efb007d112b438655dd2c897759fb1.pdf
    %AVR = -1/2 * I - 1/2 * II
    %AVL = I - 1/2 * II
    %AVF = II - 1/2 * I
    avr = -0.5 * ecg(:,1) - 0.5 * ecg(:,2);
    avl = ecg(:,1) - 0.5 * ecg(:,2);
    avf = ecg(:,2) - 0.5 * ecg(:,1);
    fprintf("Got ECG number %i!\n", i);
    matrix = tm';
    matrix = [matrix; ecg(:,7)'];
    matrix = [matrix; ecg(:,8)'];
    matrix = [matrix; ecg(:,9)'];
    matrix = [matrix; ecg(:,10)'];
    matrix = [matrix; ecg(:,11)'];
    matrix = [matrix; ecg(:,12)'];
    matrix = [matrix; ecg(:,1)'];
    matrix = [matrix; ecg(:,2)'];
    matrix = [matrix; ecg(:,3)'];
    matrix = [matrix; avr'];
    matrix = [matrix; avl'];
    matrix = [matrix; avf'];
    
    % Must have format:
    % V1 - V6, I, II, III, aVR, aVL, aVF
    filename = strcat(databases(selected_database), "_", records_trim(i), ".csv");
    writematrix(matrix, filename);
end