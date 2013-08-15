list={'R','G','B','C','M','Y'};
nrX=250;
fid=fopen('../videos/colorblind250.dat','r');
vid=fread(fid,[3*nrX*nrX inf],'uint8');
fclose(fid);
inputs = reshape(permute(reshape(vid(:,1:6)/255,3,nrX,nrX,6),[3 2 4 1 ]),[nrX,nrX*6,3]);

FrameDur=100;

resp = zeros(0,nrX*6);

for j=1:length(list)
    s = readspikes(['../V4' list{j} '.dat'],FrameDur);
    
    resp = [resp; reshape(permute(reshape(s(1:6,:),6,nrX,nrX),[3 2 1]),[nrX nrX*6])];
end
%smooth the output a little so that we average out some of the spiking noise
figure, imagesc([inputs; repmat(conv2(resp/max(resp(:)),ones(round(nrX/32),round(nrX/32)),'same')/round(nrX/32)/round(nrX/32), [1 1 3])]);
% figure, imagesc([inputs; repmat(resp/max(resp(:)), [1 1 3])]);
axis off;