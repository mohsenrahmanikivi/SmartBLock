import http.client
import re
import os
from math import ceil, floor


## Variables##########
server="192.168.137.1"
port=18332
#######################

###Func 1### get the hight from server###################
conn = http.client.HTTPConnection(server, port)
payload = "{\"method\": \"getblockcount\", \"params\": []}"
headers = {
  'Authorization': 'Basic dXNlcjpwYXNzd29yZA==',
  'Content-Type': 'text/plain'
}
conn.request("POST", "/", payload, headers)
res = conn.getresponse()
data = res.read()
data=re.findall(':([^"]*),', data.decode("utf-8"))
### RESULT
hight=int(data[0])
conn.close()


###Func Directory#######################################
headerRoot= os.getcwd()+"\HEADERS"
if not os.path.exists(headerRoot):
   os.makedirs(headerRoot)


##Func 2## Read the COUNTER.TXT##########################
counterPath = headerRoot + "/COUNTER.txt"

if os.path.isfile(counterPath):
    print("#The counter file is found")
    if os.stat(counterPath).st_size == 0:
        blockCounter=0
        
    else:
        f = open(counterPath, "r")
        blockCounter=int(f.read())
        f.close()
        print("#Counter =" , str(blockCounter) )
else:
    print("The counter file is NOT found")
    print("The counter file is Created!")
    f = open(counterPath, "w")
    f.write("0")
    blockCounter=0



#################### Header Downloader####Dependency....blockCounter....###########
endFile = ceil(hight/100)
beginFile= floor(blockCounter/100) 

for fileNumber in range(beginFile,endFile):
    conn = http.client.HTTPConnection(server, port)
    ###################Path preparation + directory ###############################
    folderName="%d_%d" % ((floor(fileNumber/1000)*10000), ((floor(fileNumber/1000)+1)*10000)-1)
    headerPath= "%s\%s" % (headerRoot, folderName) 
    if not os.path.exists(headerPath):
        os.makedirs(headerPath)
    filename="%s\%d.txt" % (headerPath, fileNumber*100)
    
    
    try:  
        if (os.path.exists(filename)) and (os.stat(filename).st_size in (22890 ,23000 , 23100 , 23200 , 23300)) :
            continue
        else :
            os.remove(filename)
            if ((fileNumber+1)*100) < hight :
                for j in range((fileNumber*100),((fileNumber+1)*100)):
                    blockCounter=j

                    ### Fetch the hash##########################################
                    payload = "{\"method\": \"getblockhash\", \"params\": [%d]}" % (blockCounter)
                    headers = {
                    'Authorization': 'Basic dXNlcjpwYXNzd29yZA==',
                    'Content-Type': 'text/plain'
                    }
                    conn.request("POST", "/", payload, headers)
                    res = conn.getresponse()
                    data = res.read()
                    data=re.findall('"([^"]*)"', data.decode("utf-8"))
                    ###RESULT
                    hash=data[1]
                    ##############################################################

                    ### Fetch the header##########################################
                    payload = "{\"method\":\"getblockheader\",\"params\":[\"%s\",false]}"% (hash)
                    headers = {
                    'Authorization': 'Basic dXNlcjpwYXNzd29yZA==',
                    'Content-Type': 'text/plain'
                    }
                    conn.request("POST", "/", payload, headers)
                    res = conn.getresponse()
                    data = res.read()
                    data=re.findall('"([^"]*)"', data.decode("utf-8"))
                    ###RESULT
                    header=data[1]
                    ############################################################
                    ## Prepare and Write in file###############################
                    ### Prepare
                    bufToWrite = "%d,%s,%s\r" % (blockCounter,hash, header)
                    
                    
                    
                    f = open(filename, "a")
                    print(filename,bufToWrite)
                    f.write(bufToWrite)
                    f.close()
                    ##########################################################
            else :
                for j in range((fileNumber*100),hight):
                    blockCounter=j

                    ### Fetch the hash##########################################
                    payload = "{\"method\": \"getblockhash\", \"params\": [%d]}" % (blockCounter)
                    headers = {
                    'Authorization': 'Basic dXNlcjpwYXNzd29yZA==',
                    'Content-Type': 'text/plain'
                    }
                    conn.request("POST", "/", payload, headers)
                    res = conn.getresponse()
                    data = res.read()
                    data=re.findall('"([^"]*)"', data.decode("utf-8"))
                    ###RESULT
                    hash=data[1]
                    ##############################################################

                    ### Fetch the header##########################################
                    payload = "{\"method\":\"getblockheader\",\"params\":[\"%s\",false]}"% (hash)
                    headers = {
                    'Authorization': 'Basic dXNlcjpwYXNzd29yZA==',
                    'Content-Type': 'text/plain'
                    }
                    conn.request("POST", "/", payload, headers)
                    res = conn.getresponse()
                    data = res.read()
                    data=re.findall('"([^"]*)"', data.decode("utf-8"))
                    ###RESULT
                    header=data[1]
                    ############################################################
                    ## Prepare and Write in file###############################
                    ### Prepare
                    bufToWrite = "%d,%s,%s\r" % (blockCounter,hash, header)
                    filename="%d.txt" % (fileNumber*100)
                                   
                    f = open(filename, "a")
                    print(filename,bufToWrite)
                    f.write(bufToWrite)
                    f.close()
                    ##########################################################

    except :
        if ((fileNumber+1)*100) < hight :
            for j in range((fileNumber*100),((fileNumber+1)*100)):
                blockCounter=j

                ### Fetch the hash##########################################
                payload = "{\"method\": \"getblockhash\", \"params\": [%d]}" % (blockCounter)
                headers = {
                'Authorization': 'Basic dXNlcjpwYXNzd29yZA==',
                'Content-Type': 'text/plain'
                }
                conn.request("POST", "/", payload, headers)
                res = conn.getresponse()
                data = res.read()
                data=re.findall('"([^"]*)"', data.decode("utf-8"))
                ###RESULT
                hash=data[1]
                ##############################################################

                ### Fetch the header##########################################
                payload = "{\"method\":\"getblockheader\",\"params\":[\"%s\",false]}"% (hash)
                headers = {
                'Authorization': 'Basic dXNlcjpwYXNzd29yZA==',
                'Content-Type': 'text/plain'
                }
                conn.request("POST", "/", payload, headers)
                res = conn.getresponse()
                data = res.read()
                data=re.findall('"([^"]*)"', data.decode("utf-8"))
                ###RESULT
                header=data[1]
                ############################################################
                ## Prepare and Write in file###############################
                ### Prepare
                bufToWrite = "%d,%s,%s\r" % (blockCounter,hash, header)
                
                ### WRITE
                # f = open(filename, "w")
                # f.close()
                
                f = open(filename, "a")
                print(filename,bufToWrite)
                f.write(bufToWrite)
                f.close()
                ##########################################################
        else :
            for j in range((fileNumber*100),hight):
                blockCounter=j

                ### Fetch the hash##########################################
                payload = "{\"method\": \"getblockhash\", \"params\": [%d]}" % (blockCounter)
                headers = {
                'Authorization': 'Basic dXNlcjpwYXNzd29yZA==',
                'Content-Type': 'text/plain'
                }
                conn.request("POST", "/", payload, headers)
                res = conn.getresponse()
                data = res.read()
                data=re.findall('"([^"]*)"', data.decode("utf-8"))
                ###RESULT
                hash=data[1]
                ##############################################################

                ### Fetch the header##########################################
                payload = "{\"method\":\"getblockheader\",\"params\":[\"%s\",false]}"% (hash)
                headers = {
                'Authorization': 'Basic dXNlcjpwYXNzd29yZA==',
                'Content-Type': 'text/plain'
                }
                conn.request("POST", "/", payload, headers)
                res = conn.getresponse()
                data = res.read()
                data=re.findall('"([^"]*)"', data.decode("utf-8"))
                ###RESULT
                header=data[1]
                ############################################################
                ## Prepare and Write in file###############################
                ### Prepare
                bufToWrite = "%d,%s,%s\r" % (blockCounter,hash, header)
                filename="%d.txt" % (fileNumber*100)
                ### WRITE
                # f = open(filename, "w")
                # f.close()
            
                f = open(filename, "a")
                print(filename,bufToWrite)
                f.write(bufToWrite)
                f.close()
                ##########################################################

   
    ### WRITE blockCounter to file#####
    f = open(counterPath, "w")
    f.write(str(blockCounter))
    f.close()
    ###################################

    conn.close()





print("Finished")


