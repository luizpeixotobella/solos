# LinkedIn draft — SolOS update

O SolOS continua avançando, mas achei importante fazer um esclarecimento técnico simples sobre o estado atual do projeto.

Hoje, a base já não é apenas uma interface com textos soltos espalhados pela UI. O projeto já tem uma primeira fronteira arquitetural real: um runtime core em Rust gera um snapshot estruturado de estado, o C++/Qt importa esse contrato, coordena modelos e estados da shell, e o QML renderiza isso como superfície nativa.

Ao mesmo tempo, também seria exagero dizer que isso já é um sistema amplamente dinâmico. Boa parte do estado ainda é pré-definido. Então a descrição mais honesta, para mim, é esta: o SolOS já saiu do puro mock estático, mas ainda está em transição de protótipo estruturado para sistema real.

Esse é justamente o próximo passo da nova guinada do projeto: transformar esse seam atual em runtime de verdade, com estado mais vivo, approvals executáveis, integração de wallet/account, fronteiras mais claras para apps e uma presença de agente cada vez menos cosmética e mais operacional.

Em essência, o projeto continua perseguindo a mesma tese: construir uma operating layer para agents, dApps e identidade digital. A diferença é que agora a arquitetura começa a ficar mais explícita, mais auditável e mais honesta sobre onde está e para onde vai.

Repositório público:
https://github.com/luizpeixotobella/solos.git
