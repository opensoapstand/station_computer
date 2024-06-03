import os
import requests
import urllib.parse
import logging

# Configure logging
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')

# Load configuration from environment variables
API_URL = os.environ.get('API_URL', 'https://soapstandportal.com/api/order/sendCompleteNewOrder')
PING_URL = os.environ.get('PING_URL', 'https://soapstandportal.com/api/machine_data/ping')
TRANSACTION_FILE = os.environ.get('TRANSACTION_FILE', 'logging/transactions/failed_curl_transaction_dispense_end.txt')

def parse_query_string(query_string):
    try:
        return urllib.parse.parse_qs(query_string.replace(' & ', ' and '))
    except ValueError:
        logging.error(f"Invalid query string: {query_string}")
        return None

def push_transactions():
    with open(TRANSACTION_FILE, 'r+') as file:
        transactions = file.readlines()
        successful_transactions = []
        for transaction in transactions:
            parsed_transaction = parse_query_string(transaction.strip())
            if parsed_transaction:
                try:
                    response = requests.post(API_URL, data=parsed_transaction)
                    response.raise_for_status()
                    logging.info(f"Transaction sent successfully: {transaction.strip()}")
                    successful_transactions.append(transaction)
                except requests.exceptions.RequestException as e:
                    logging.error(f"Error sending transaction: {e}")
        file.seek(0)
        file.writelines(line for line in transactions if line not in successful_transactions)
        file.truncate()

def ping_server():
    try:
        response = requests.get(PING_URL)
        response.raise_for_status()
        return response.text == "Connected"
    except requests.exceptions.RequestException as e:
        logging.error(f"Error pinging server: {e}")
        return False

if __name__ == "__main__":
    if ping_server():
        logging.info("Server is reachable, pushing transactions")
        push_transactions()
    else:
        logging.warning("Server is not reachable, skipping transaction push")