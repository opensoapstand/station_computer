crontab -l | grep -q 'missingTransactions.py' && echo 'Already exists'  \
|| crontab -l > mycron 
echo "0 * * * *  python ~/production/admin/missingTransactions.py" >> mycron 
crontab mycron
rm mycron