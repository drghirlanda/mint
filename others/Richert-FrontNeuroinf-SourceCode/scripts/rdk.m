% ss = zeros(8,10,8);
% 
% frameDur = 3300;
% 
% s = readspikes('../PFC.dat',3300);
% if (size(s,1) < 80) s(80,8) = 0; end
% 
% ss = squeeze(mean(reshape(s,8,10,[],8),3));
% figure, plot(100:-10:10,diff(squeeze(ss(3,:,[1 5]+2)),[],2)*1000/frameDur); 
% xlabel('Coherence'); 
% ylabel('PFC right - left (Hz)'); 
% title('Motion Selectivity in PFC');


s = readspikes('../PFC.dat',1);
s(8*10*frameDur,1) = 0; % grow it to the right size...

s = reshape(full(s(1:8*10*frameDur,:)),3300, 8, 10, [], 8);

correct = zeros(8,10,size(s,4));
times = zeros(8,10,size(s,4));

measureAtSpike = 10;

for i=1:8
    for c=1:10
        for j=1:size(s,4)
            spikeTimesPref = [find(s(:,i,c,j,mod(i+4-1,8)+1)); repmat(inf,measureAtSpike,1)];
            spikeTimesAntiPref = [find(s(:,i,c,j,mod(i-1,8)+1)); repmat(inf,measureAtSpike,1)];
            
            correct(i,c,j) = spikeTimesPref(measureAtSpike)<spikeTimesAntiPref(measureAtSpike);
            times(i,c,j) = min(spikeTimesPref(measureAtSpike),spikeTimesAntiPref(measureAtSpike));
        end
    end
end

perCorrect = mean(correct,3);
figure, plot(100:-10:0,[mean(perCorrect) 0.5]);
xlabel('Coherence'); 
ylabel('Percent Correct'); 

mTimes = mean(times,3);
figure, plot(100:-10:10,mTimes(3,:)/1000);
xlabel('Coherence'); 
ylabel('Reaction Time'); 
