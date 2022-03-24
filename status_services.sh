#!/usr/bin/env bash
echo "controller:"
systemctl is-active controller_soapstand.service
systemctl is-enabled controller_soapstand.service
echo "ui:"
systemctl is-active ui_soapstand.service
systemctl is-enabled ui_soapstand.service
echo "reversed ssh tunnel:"
systemctl is-active rtunnel.service
systemctl is-enabled rtunnel.service
# systemctl status controller_soapstand.service
# systemctl status ui_soapstand.service
# systemctl status rtunnel.service
