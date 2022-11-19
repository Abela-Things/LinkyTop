# LinkyTop
Un projet avec ESP-01 et Node-Red pour surveiller votre consommation électrique en temps réel via une interface web.

## Matériel
### Minimum compatible
* devkit esp8266
* Un adaptateur pour convertir la sortie TIC (Télé-Information Client) du compteur en UART lisible par le micro-contrôleur. J'ai choisi celui-ci sur Tindie.com : https://www.tindie.com/products/hallard/pitinfo/ Le circuit n'est pas bien complexe, et vous pourriez le réaliser vous même sans l'acheter. Je l'ai pris parce-que j'aime bien quand mes circuits tiennent sur une petite breadboard.
* Un Raspberry Pi ou juste votre PC perso. Si vous comptez le garder installé à long terme, je vous recommande d'utiliser une machine qui pourra rester tout le temps allumée chez vous, ou votre propre serveur cloud, mais dans ce dernier cas, vous devrez apporter des modifications supplémentaires au code pour sécuriser les communications (HTTPS, et MQTT chiffré). Pour ma part j'ai déjà un serveur multi-usages à la maison, j'ai juste ajouté les services nécessaires dessus (Broker MQTT, et nodered).
### Materiel actuellement utilisé chez moi
* ESP-01S moddé pour deep-sleep (GPIO15 manuellement soudé à RST pour l'horloge puisse réveiller le contrôleur à la fin de la periode de deep-sleep définie)
* 2 batteries Li-Ion 3.7V de 4000mAh chacune
* Alimentation à découpage 3.3V (Très efficace sur le papier si on consomme beaucoup d'énergie, mais celle que j'utilise consomme 1mA en continu, donc c'est pas idéal pour une application basse-consomation.)
* L'adaptateur cité plus haut \
Attention, ce montage avec l'alimentation 3.3V risque de trop décharger les batteries, dans mon cas c'est une installation temporaire et je vérifie régulèrement la tension des batteries pour les protéger, mais idéalement il faudrait ajouter un circuit de protection en plus.

## Arduino
Le code dans LinkyTop.ino va se connecter au réseau Wifi défini (Une fonctionnalité pour configurer la connexion wifi via un serveur Web hébergé sur l'ESP sera bientôt ajoutée). Vous devrez installer la bibliothèque **PubSubClient de Nick O'Leary**, normalement les autres sont déjà présentes par défaut. Il y a un peu de configuration à faire pour pouvoir programmer un ESP 8266 avec l'Arduino IDE. Ça fait un moment que j'ai fait ça, donc je vous laisse chercher pour cette partie. \
J'ai joint un dump de la sortie de mon compteur, j'avais récupéré ça au début du projet et ça m'a été utile pour savoir comment le parser. On y remarque que l'info de consommation en temps réel PAPP est exprimée en VA, et que les compteurs de consommation sont en W. Chez moi, mon compteur ne me donne que des multiples de 10 pour la valeur PAPP. C'est pas très précis mais c'est déjà très utile.

## Node-Red
Les "flows" nécessaires au fonctionnement du projet et au dashboard sont définis dans le fichier **flows.json**, vous pourrez l'importer vous-même depuis l'interface de node-red une fois que vous l'aurez installé. J'ai joint mon fichier de configuration aussi, mais à part pour gérer les comptes utilisateurs, je ne m'en suis pas vraiment servi. Ça ne fonctionnera pas out-of-the-box parce-que vous devrez installer vous-même nodered et les dépendances définies dans **package.json** (installables depuis l'interface web de nodered ou via npm), et vous devrez configurer la connexion MQTT et mongo avec vos propres serveurs.

## Données CO2 et origine de l'électricité du réseau Français: electricitymaps.com

## Limitations et améliorations possibles
* Le compteur Linky peut allimenter les appareils qui récupèrent les infos de la sortie TIC, mais seuls 150mW sont disponibles ce qui est bien trop peu pour l'ESP-01. C'est un projet simple à mettre en place grâce à l'ESP, mais une solution avec une radio de faible puissance à la place de l'ESP pourrait être alimentée et ne pas nécessiter de batterie et d'entretien.
* Affichage du statut EcoWatt via l'API RTE dédiée.
* Déploiement automatique des serveurs MQTT et Node-red
* Configuration du Wifi non-hardcodée via une interface web
