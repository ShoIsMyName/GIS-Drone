import cv2
import os
from colorama import Fore


################################################
#####                                      #####
#####          SETTING ALL HERE            #####
#####                                      #####
################################################

folderPath = "./ImageFolder"
resultFolderPath = "./ResultMapFolder"
verbose = True



try:
    contents = os.listdir(folderPath)
except:
    if verbose:
        print(f"[{Fore.RED}X{Fore.RESET}] No directory name \"{folderPath}\".")
    os._exit(1)

imageList = []

try:
    # append images (not filenames) to imageList
    for item in contents:
        full_path = os.path.join(folderPath, item)
        if os.path.isfile(full_path):
            img = cv2.imread(full_path)
            if img is not None:
                imageList.append(img)
except:
    if verbose:
        print(f"[{Fore.RED}X{Fore.RESET}] Can't import any file from \"{folderPath}\" directory.")
    os._exit(1)

try:
    # stitcher the picture into map
    stitcher = cv2.Stitcher_create()
    status, resultMap = stitcher.stitch(imageList)
except:
    if verbose:
        print(f"[{Fore.RED}X{Fore.RESET}] Error to stitcher.")
    os._exit(1)

# [FUNCTION] Check length file in directory
def count_files_in_directory(directory_path):
    count = 0
    for item in os.listdir(directory_path):
        item_path = os.path.join(directory_path, item)
        if os.path.isfile(item_path):
            count += 1
    return count

sequenceFileInDirectoryNumber = count_files_in_directory(resultFolderPath)

if status == cv2.Stitcher_OK:
    if verbose:
        cv2.imwrite(f"./ResultMapFolder/resultMap{sequenceFileInDirectoryNumber+1}.jpg", resultMap)
    print(f"[{Fore.GREEN}#{Fore.RESET}] Stitching complete!")
else:
    if verbose:
        print(f"[{Fore.RED}X{Fore.RESET}] Stitching failed: ", status)
