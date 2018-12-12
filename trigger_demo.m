[y,Fs]=audioread('triggerbox_demo_tester.wav'); %Contains two channels, left is sine wave and right is trigger pulse sent to trigger box
scaled_y = y;

%%

s1 = serial('COM9','BaudRate',9600);
fopen(s1);

%%
trigger_types = randi(5,1000,1); %Do 1000 trials with 5 different levels
%%
for i = 1:1000
    
    trigger_number = trigger_types(i);

    fwrite(s1,trigger_number,'uint8');
    
    pause(0.05); %Give the teensy program some time (50ms) to process input and be ready to trigger
    
    try
        char(fread(s1,s1.BytesAvailable)') %Display communications between teensy and Matlab
    catch
        disp('No serial comms available')
    end
    
    scaled_y(:,1) = y(:,1)/trigger_number; %Just some scaling so we get different outputs every time
    
    sound(scaled_y,Fs);
    pause(.5) %This line MUST be here since the sound function does not block execution of the script
    
    try
        char(fread(s1,s1.BytesAvailable)')
    catch
        disp('No serial comms available')
    end
    
    
end
%%

fclose(s1);
delete(s1);
clear s1
