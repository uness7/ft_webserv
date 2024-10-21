#!/bin/bash

# Contenu HTML à renvoyer
html_content="<html>
<head>
    <meta charset='UTF-8'>
    <title>Réponse CGI</title>
</head>
<body>
    <h1>Succès</h1>
    <p>La réponse a été générée avec succès.</p>
</body>
</html>"

# Calculer la longueur du contenu
content_length=${#html_content}

# Indiquer que le script est exécuté en tant que CGI
echo "HTTP/1.1 200 OK"
echo "Content-Type: text/html"
echo "Content-Length: $content_length"
echo ""  # Ligne vide pour séparer les en-têtes du corps

# Envoyer le contenu HTML
echo "$html_content"
