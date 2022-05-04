#!/usr/bin/env bash
# echo "controller:"
# systemctl is-active controller_soapstand.service
# systemctl is-enabled controller_soapstand.service
# echo "ui:"
# systemctl is-active ui_soapstand.service
# systemctl is-enabled ui_soapstand.service
# echo "reversed ssh tunnel:"
# systemctl is-active rtunnel.service
# systemctl is-enabled rtunnel.service
# systemctl status controller_soapstand.service
# systemctl status ui_soapstand.service
# systemctl status rtunnel.service
echo -e "  Service Name      |  Running?\t|  Autostart?   "
active=$(systemctl is-active controller_soapstand.service)
enabled=$(systemctl is-enabled controller_soapstand.service)
echo -e "controller          |$active  \t|$enabled "
active=$(systemctl is-active ui_soapstand.service)
enabled=$(systemctl is-enabled ui_soapstand.service)
echo -e "ui                  |$active  \t|$enabled"
active=$(systemctl is-active ui_soapstand.service)
enabled=$(systemctl is-enabled ui_soapstand.service)
echo -e "reversed ssh tunnel |$active  \t|$enabled"
