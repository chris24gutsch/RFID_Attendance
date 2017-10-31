%% ADD %
%FIX DATETIME%
    %Set logical expression?%
    %???%
%Automate registration%
    %ThingSpeak? Automate Students.csv?%
%Batch file to run script and open attendance sheet ?%

%% Variables %%
%Input the Channel ID for your ThingSpeak Server
readChannelID = 352563;
% Enter your Read API key (' ')
readAPIKey = 'IV6EC6SOZP8F818R';

%Input the Channel ID for your ThingSpeak Server
writeChannelID = 352563;
% Enter your Write API key (' ')
writeAPIKey = 'ZGUH9PMYN82B6TY4';
%% Read Data %%
% Pulls data from ThingSpeak API. NumPoints is changeable. Timetable
%can typically be used. Threw excpetions here. Table is changed to a timetable. 
data = thingSpeakRead(readChannelID, 'ReadKey', readAPIKey, 'Fields',1,'NumPoints',100,'outputFormat','table');

%Prints raw ThingSpeak table for debugging
%data;

%Changes data from table to timetable. Creates easier filtering
newdata = table2timetable(data);

%Prints timetable for debugging
%newdata;
%% FILTERING %%

%Filters time range from datestr to now. 
%%%%%%%NEEDS TO BE EDITED%%%%%%%
S = timerange(datestr('2017-10-24 11:10:00', 31), datetime('now'));

%Creates new timetable with the filtered time range S and rfid
filteredData = newdata(S,:);

%Prints timetable for debugging
%filteredData;

%% STUDENT KEY TABLE %% 

% reads table with student keys
% must be in directory of .m file
% can be read as text (.csv, .txt, .dat) or a spreadsheet
T = readtable('students.csv');

% Address table by the tag IDs
% Tags must be in column 3 or (:, 3) must be changed
tagIDs = table2cell(T(:, 3));
% Sets the table to be addressed by tagIDs
T.Properties.RowNames = tagIDs;

% Add column called "Present" and set to zero
T.Present = zeros(height(T), 1);
% Add a columnt called "Date" and put today's date in
T.Date = repmat({datestr(now, 02)}, height(T), 1); 

% For every filtered tag ID we've found, mark the student present
%  This is accomplished by setting the 'Present' field to 1
for i = 1:length(filteredData.rfid)
    try
        epc = filteredData.rfid{i};      
        T({epc}, :).Present = 1;
    catch ME
        fileID = fopen('log.txt'); %%opens log file for errors%%
        error = warning(ME.identifier, '%s', ME.message);  %catches exceptions%
        fprintf(fileID, error); %%prints to log file%%
        fclose(fileID); %%closes log%%
        continue
    end
end             

% Make a copy but only save Last name, First name, Present and Date
Tout = T(:, {'lastName', 'firstName', 'Present', 'Date'});

% Write this output to a file 
% Can be outputted to a text (.txt, .dat, .csv) file or 
% Excel (xls,.xlsm,.xlsx) file
writetable(Tout, 'out.xlsx')