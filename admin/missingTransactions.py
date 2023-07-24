import requests
import json

def convertStringToDictionary(stringObj):
    newString = '{"'
    if b'\\00\\00' in stringObj:
        return "Invalid"
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
url = 'https://soapstandportal.com/api/order/addMissingTransactions'
pingUrl = 'https://soapstandportal.com/api/machine_data/ping'

def pushTransaction():
    #Open the file and read the file line by line
    with open(filename,'r') as file1:
        transactions = file1.readlines()
        while transactions:
            transaction = convertStringToDictionary(transactions[0].strip().replace(" & ", " and "))
            if transaction:
                if transaction =="Invalid":
                    transactions.pop(0)
                x = requests.post(url, transaction)
                if x.text:
                    transactions.pop(0)

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

