import requests
import json

def convertStringToDictionary(stringObj):
    newString = '{"'
    for i in stringObj:
        if i=='&':
            newString+= '","'
        elif i=='=':
            newString += '":"'
        else:
            newString += i
    newString += '"}'
    return json.loads(newString)

#Declare file name
filename = 'logging/transactions/missingTransactions.txt'
url = 'https://soapstandportal.com/api/machine_data/pushPrinterOrder'
pingUrl = 'https://soapstandportal.com/api/machine_data/ping'

def pushTransaction():
    #Open the file and read the file line by line
    with open(filename,'r') as file1:
        transactions = file1.readlines()
        while transactions:
            transaction = convertStringToDictionary(transactions[0].strip())
            if transaction:
                x = requests.post(url, transaction)
                if x.text:
                    transactions.pop()
                else:
                    break

    #Empty the file if all the records are successful
    open(filename,'w').close()

    #Add the missing records if Internet is down again
    with open(filename,'w') as writeToFile:
        writeToFile.write("\n".join(transaction.strip() for transaction in transactions))

try:
    x = requests.get(pingUrl)
    if x.text == "Connected":
        pushTransaction()
except:
    pass