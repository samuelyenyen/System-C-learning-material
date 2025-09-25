% 打開文件
fileID = fopen('cat.txt','r');

% 假設我們知道文件格式並且可以直接讀取數據
% 這裡的讀取方法完全取決於你的txt文件是如何組織數據的
data = fscanf(fileID, '%f', [224*224*3, 1]);

% 關閉文件
fclose(fileID);

% 將讀取的數據轉換為224*224*3的格式
imageData = reshape(data, [224, 224, 3]);
imageData(:,:,1) = imageData(:,:,1)';
imageData(:,:,2) = imageData(:,:,2)';
imageData(:,:,3) = imageData(:,:,3)';

% 將讀取的數據轉換為224*224*3的格式
minVal = min(imageData(:));
maxVal = max(imageData(:));
normalizedData = (imageData - minVal) / (maxVal - minVal);
finalData = uint8(255 * normalizedData);

% 顯示圖片
figure(1)
imshow(finalData);
%%
% 打開文件
fileID = fopen('resized_cat.txt','r');

% 假設我們知道文件格式並且可以直接讀取數據
% 這裡的讀取方法完全取決於你的txt文件是如何組織數據的
data = fscanf(fileID, '%f', [227*227*3, 1]);

% 關閉文件
fclose(fileID);

% 將讀取的數據轉換為224*224*3的格式
imageData1 = reshape(data, [227, 227, 3]);
imageData1(:,:,1) = imageData1(:,:,1)';
imageData1(:,:,2) = imageData1(:,:,2)';
imageData1(:,:,3) = imageData1(:,:,3)';

% 將讀取的數據轉換為224*224*3的格式
minVal = min(imageData1(:));
maxVal = max(imageData1(:));
normalizedData = (imageData1 - minVal) / (maxVal - minVal);
finalData1 = uint8(255 * normalizedData);

% 顯示圖片
figure(2)
imshow(finalData1);