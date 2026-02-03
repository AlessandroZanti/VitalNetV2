#!/bin/bash

# Ensure the script is run as root
if [ "$EUID" -ne 0 ]; then
  echo "Please run as root (use sudo)"
  exit 1
fi

echo "--- Installing Dependencies ---"
apt-get update
apt-get install -y apache2

echo "--- Enabling Apache Modules ---"
a2enmod rewrite

echo "--- Setting up VitalNet Directory ---"
# Create destination directory
mkdir -p /usr/local/VitalNet

# Copy all files from the current directory to /usr/local/VitalNet
# We use . to copy the content of BouyReceiver-main into VitalNet
cp -r ./* /usr/local/VitalNet/
cp -r ./.[!.]* /usr/local/VitalNet/ 2>/dev/null || true # Copy hidden files too

# Set permissions
chown -R www-data:www-data /usr/local/VitalNet
chmod -R 755 /usr/local/VitalNet

echo "--- Configuring Apache Site ---"
# Create the vitalnet.conf file
cat <<EOF > /etc/apache2/sites-available/vitalnet.conf
<VirtualHost *:8081>
    ServerAdmin webmaster@localhost
    DocumentRoot /usr/local/VitalNet/data/pages

    <Directory /usr/local/VitalNet/data/pages>
        Options Indexes FollowSymLinks
        AllowOverride All
        Require all granted
        
        # Start at login.html by default
        DirectoryIndex login.html

        # Enable rewriting for clean URLs (e.g. /inicio -> /inicio.html)
        RewriteEngine On
        RewriteCond %{REQUEST_FILENAME} !-d
        RewriteCond %{REQUEST_FILENAME}.html -f
        RewriteRule ^(.*)$ \$1.html [L]
    </Directory>

    ErrorLog \${APACHE_LOG_DIR}/vitalnet_error.log
    CustomLog \${APACHE_LOG_DIR}/vitalnet_access.log combined
</VirtualHost>
EOF

echo "--- Configuring Apache Ports ---"
# Check if Listen 8081 is already in ports.conf, if not add it
if ! grep -q "Listen 8081" /etc/apache2/ports.conf; then
    echo "Listen 8081" >> /etc/apache2/ports.conf
fi

echo "--- Activating VitalNet Site ---"
# Disable default if necessary (optional)
# a2dissite 000-default.conf

# Enable vitalnet site
a2ensite vitalnet.conf

echo "--- Restarting Apache ---"
systemctl restart apache2

echo "--- Installation Complete ---"
echo "VitalNet is now running on http://localhost:8081"
