Společná část popisu:
Vytvořte komunikující aplikaci podle konkrétní vybrané specifikace pomocí síťové knihovny BSD sockets (pokud není ve variantě zadání uvedeno jinak). Projekt bude
vypracován v jazyce C/C++, přeložitelný a spustitelný v prostředí systému Unixového typu (testováno bude na virtuálním počítači Linux/Ubuntu
http://nes.fit.vutbr.cz/isa/ISA2015.ova).

    Vypracovaný projekt uložený v archívu .tar a se jménem xlogin00.tar odevzdejte elektronicky přes IS. Soubor nekomprimujte.
    Termín odevzdání je 20.11.2016 (hard deadline). Odevzdání emailem po uplynutí termínu není možné.
    Odevzdaný projekt musí obsahovat:
        soubor se zdrojovým kódem (dodržujte jména souborů uvedená v konkrétním zadání),
        funkční Makefile pro překlad zdrojového souboru,
        dokumentaci (soubor manual.pdf), která bude obsahovat uvedení do problematiky, návrhu aplikace, popis implementace, základní informace o programu,
        návod na použití. V dokumentaci se očekává následující: titulní strana, obsah, logické strukturování textu, přehled nastudovaných informací z literatury,
        popis zajímavějších pasáží implementace, použití vytvořených programů a literatura.
        soubor README obsahující krátký textový popis programu s případnými rozšířeními/omezeními, příklad spuštění a seznam odevzdaných souborů,
        další požadované soubory podle konkrétního typu zadání.
    Pokud v projektu nestihnete implementovat všechny požadované vlastnosti, je nutné veškerá omezení jasně uvést v dokumentaci a v souboru README.
    Co není v zadání jednoznačně uvedeno, můžete implementovat podle svého vlastního výběru. Zvolené řešení popište v dokumentaci.
    Při řešení projektu respektujte zvyklosti zavedené v OS Unixového typu (jako je například formát textového souboru).
    Vytvořené programy by měly být použitelné a smysluplné, řádně komentované a formátované a členěné do funkcí a modulů. Program by měl obsahovat nápovědu
    informující uživatele o činnosti programu a jeho parametrech. Případné chyby budou intuitivně popisovány uživateli.
    Aplikace nesmí v žádném případě skončit s chybou SEGMENTATION FAULT ani jiným násilným systémovým ukončením (např. dělení nulou).
    Pokud přejímáte velmi krátké pasáže zdrojových kódů z různých tutoriálů či příkladů z Internetu (ne mezi sebou), tak je nutné vyznačit tyto sekce a jejich autory dle
    licenčních podmínek, kterými se distribuce daných zdrojových kódů řídí. V případě nedodržení bude na projekt nahlíženo jako na plagiát.
    Konzultace k projektu podává vyučující, který zadání vypsal.
    Před odevzdáním zkontrolujte, zda jste dodrželi všechna jména souborů požadovaná ve společné části zadání i v zadání pro konkrétní projekt.
    Zkontrolujte, zda je projekt přeložitelný.

Hodnocení projektu:

    Maximální počet bodů za projekt je 15 bodů.
    Příklad kriterií pro hodnocení projektů:
        nepřehledný, nekomentovaný zdrojový text: až -7 bodů
        nefunkční či chybějící Makefile: až -4 body
        nekvalitní či chybějící dokumentace: až -5 bodů
        nedodržení formátu vstupu/výstupu či konfigurace: -3 body
        odevzdaný soubor nelze přeložit, spustit a odzkoušet: 0 bodů
        odevzdáno po termínu: 0 bodů
        nedodržení zadání: 0 bodů
        nefunkční kód: 0 bodů
        opsáno: 0 bodů (pro všechny, kdo mají stejný kód)


Instrukce pro spuštění virtuálního stroje:

Virtuální stroj byl vytvořen programem VirtualBox. Pro import a spuštění ve VMWare postupujte podle návodů pro import virtuálních strojů vytvořených programem VirtualBox.
Pokud budete mít problém se spuštěním systému, ověřte, že máte povoleno (VT-x, AMD-V apod.) v Bios. Pokud Váš CPU nepodporuje VT-x,
zakažte jej v nastavení virtuální stanice: Nastavení -> Systém -> Akcelerace (VT-x, AMD-V).

Pro zprovoznění virtuálního stroje není nutně potřeba hardwarová akcelerace. Pokud máte po přihlášení problémy s načtením výchozího prostředí Unity (černá obrazovka,
automatický návrat na přihlašovací obrazovku, apod.), můžete použít méně graficky náročné prostředí Metacity. Pro zprovoznění Metacity lze použít např. následující postup:

- Na přihlašovací obrazovce přejděte na textový terminál (např.
stiskem Ctrl+Alt+F1).
- Přihlašte se přes terminál zadáním uživatelského jména a heslo.
- Přihlašovací údaje jsou:
username: isa2015
password: isa2015
- Napište "sudo apt-get install gnome-session-fallback", zadejte heslo ještě jednou
a pomocí "y" potvrďte instalaci balíčků.
- Po nainstalování zadejte "sudo reboot"
- Při novém startu systému ještě před přihlášením klikněte na bílou
ikonku s logem ubuntu a vyberte prostředí Metacity místo
výchozího Unity.
- Nyní se můžete přihlásit.

Popis varianty:
DHCP server

Vaším úkolem bude naimplementovat program dserver, který bude fungovat jako jednoduchý DHCP server pro IPv4 v jazyce C/C++.
Server poběží na standardním portu a bude podporovat zprávy DISCOVER, OFFER, REQUEST, ACK, NACK a RELEASE.
Program bude podporovat parametry -p a -e, které jsou specifikovány níže.
V případě, že se klient pokusí obnovit přiřazenou IP adresu před vypršením limitu, server by mu měl danou IP adresu ponechat.

Program se ukončí po obdržení signálu SIGINT.

Příklad spuštění

./dserver -p 192.168.0.0/24 [-e 192.168.0.1,192.168.0.2]

Parametry
    -p <ip_addresa/maska>    rozsah přidělovaných IP adres
    -e <ip_addresy>             adresy z daného rozsahu, které se nepřiřazují žádným klientům (oddělené čárkou)

Parametr -p musí být specifikován vždy, parametr -e je volitelný. Lease time nastavte vždy na 1 hodinu.

Výstup
Pro každého klienta, kterému přiřadíte IP adresu, bude na stdout vypsaný řádek
<mac_adresa> <ip_addresa> <cas_prideleni_ip_adresy> <cas_vyprseni_prirazeni_adresy>

například:
c8:0a:a9:cd:7d:81 192.168.0.101 2016-09-29_13:45 2016-09-29_15:45

Při obnovení IP adresy vypište stejný řádek s aktualizovaným časem vypršení přiřazení IP adresy.

Poznámky k implementaci

    při vytváření programu je povoleno použít hlavičkové soubory pro práci se sokety a další obvyklé funkce používané
    v síťovém prostředí (jako je netinet/*, sys/*, arpa/* apod.), knihovnu pro práci s vlákny (pthread), signály, časem,
    stejně jako standardní knihovnu jazyka C (varianty ISO/ANSI i POSIX), C++ a STL
    použití jakýchkoliv jiných knihoven je vždy nutné konzultovat
    pořádně si přečtěte společné zadání ke všem projektům a dejte si pozor na správné pojmenování všech souborů
    dotazy směřujte především do fóra, pomůžete tím i spolužákům

Testování
Pro účely testování lze využít image pro VirtualBox, který je dostupný na adrese http://nes.fit.vutbr.cz/isa/ISA2015.ova

Rozšíření
Rozšířením projektu může být parametr -s (volitelný), který umožní statické mapování IP adres. Program se pak bude spouštět například takto:

./dserver -p 192.168.0.0/24 -s static.txt

    -s <jmeno_souboru>       soubor se statickými alokacemi (seznam MAC adres a IP adres, které se k nim budou přiřazovat)

Obsah souboru static.txt (například):
00:0b:82:01:fc:42 192.168.0.99
c8:0a:a9:cd:7d:81 192.168.0.101

Za rozšíření je možné získat až 5 bodů.

Literatura

    https://www.ietf.org/rfc/rfc2131.txt
    http://markgossa.blogspot.cz/2015/08/dhcp-client-behaviour.html
