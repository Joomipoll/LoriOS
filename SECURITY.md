# Security Policy
1. **GENERAL POSITION**
    1. This information security policy (hereinafter - ISP) is approved by the head of volant127
    and defines the activities, procedures and rules for protecting information in information systems of volant127.
    2. The provisions of this Policy may be information system volant127:
        - ISPD "volant open repositories".
    3. The provisions of this Policy are binding on all users of the information systems specified in paragraph 2 (hereinafter referred to as Users),
    as well as for security administrators and system administrators (hereinafter referred to as Administrators).
    4. The objectives of this policy are:
        - Ensuring that a message is sent about a vulnerability in a media product;
        - Monitoring security events and responding to security observations;
        - Neutralization of actual threats to information security.
2. **RULES AND PROCEDURES FOR IDENTIFYING, ANALYZING AND REMOVING VULNERABILITIES**
    1. volant127 uses the following scanners as a means of detecting vulnerabilities:
        - Nmap;
        - Falcon;
        - Burpsuite;
        - Metasploit Pro;
    2. Developers can perform a full system scan for vulnerabilities.
    In case of receipt of information from news sources about vulnerabilities in operating systems and / or application software used in volant127,
    an unscheduled update of the vulnerability scanner database and a full scan of the information system are performed.
    3. The administrators examines the reports on the results of the scan and makes a decision to immediately eliminate the identified vulnerabilities,
    or to include measures to eliminate the identified vulnerabilities in the information protection action plan if the identified vulnerabilities are not critical,
    or if it is possible to make them impossible for a potential attacker to exploit
    (for example, by disconnecting individual workstations and / or network segments from the Internet).
    If necessary, the CERT may be convened to adequately respond to newly identified threats.
    4. The criticality of vulnerabilities can be established as the origin of the vulnerability rating on the CVSS scale
    5. When vulnerabilities are identified, developers analyze system logs and logs of information security tools to identify the exploitation of the identified vulnerability in the information system and the consequences of such exploitation.
    6. If it is impossible to promptly eliminate a critical vulnerability, the developer notifies the head of the vulnerability analysis department, volant127 (KaliBebra), about this.
    7. In case of problems using the product and/or discovering vulnerabilities, report it to the section "Issues".

## Supported Versions

| Version | Supported          |
| ------- | ------------------ |
| 1.0     | :white_check_mark: |
