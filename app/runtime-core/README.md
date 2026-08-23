# SolOS Runtime Core

Primeiro embrião do **runtime intermediário** do SolOS em Rust.

## Propósito

Em vez de reescrever a shell nativa inteira, este crate define o primeiro boundary útil entre:

- Linux como base do sistema
- runtime como camada intermediária de mediação
- SolOS como operating layer acima dessa mediação

O papel deste crate é:

- ler fatos e capacidades do host Linux
- normalizar esse estado em contratos estáveis para o SolOS
- preparar mediação de serviços, processos, sessões e approvals
- desacoplar a operating layer dos detalhes crus do host

## Forma interna

O runtime segue um monólito modular: um crate e um Daemon persistente, com responsabilidades internas separadas antes de qualquer extração de processos.

- `host_runtime.rs` — descoberta e normalização de fatos do Linux;
- `surface_catalog.rs` — catálogo de apps, approvals e ações rápidas expostas à operating layer;
- `main.rs` — composição compatível do snapshot RC1 enquanto Ghost, Wallet/quota e contracts são extraídos progressivamente;
- `bin/solos-daemon.rs` — processo persistente e protocolo local owner-only.

Regra: **coeso por fora, modular por dentro**. O snapshot agregado continua como read model de compatibilidade, não como justificativa para manter todos os domínios num único arquivo.

## Doutrina do Ghost

O Ghost não deve ser entendido como um chatbot colado na shell. Ele está evoluindo como uma camada de inteligência do SolOS baseada na inversão da programação clássica.

Na programação convencional, a fórmula comum é:

```text
algoritmos + dados = resultados
```

No eixo de IA que orienta o Ghost, a fórmula passa a ser:

```text
dados + resultados = algoritmos
```

Ou seja: dados locais, evidências externas, resultados desejados, respostas aceitas, citações, aprovações e rejeições devem alimentar a síntese progressiva de comportamento. O runtime-core começa essa direção expondo camadas como `data`, `results`, `algorithms` e `knowledge`, preparando o Ghost para agir como um sistema em camadas, inspirado pela linhagem conceitual do perceptron e das redes neurais, sem fingir que toda automação inteligente precisa ser machine learning.

O snapshot agora também expõe `ghost.operationalReadiness`. Essa camada mede se o Ghost está preparado para operar como agente dentro do SolOS, cobrindo:

- pesquisa grounded/RAG
- memória local de longo prazo
- fronteira de ferramentas e MCP
- aprovações humanas
- observabilidade, traces e evals
- mediação de idioma e tom

Isso evita confundir presença com autonomia. O Ghost pode estar visível antes de estar autorizado a executar ações sensíveis.

O snapshot agora também expõe a primeira camada de classificação e trace:

- `ghost.requestClassifier`
- `ghost.actionTrace`
- `ghost.routeExplanation`

Essa camada mostra classe do pedido, nível de segurança, ferramentas necessárias, aprovação, custo de cota, rota escolhida e resultado esperado antes de o Ghost sugerir execução.

## O que este crate não é

Ele não é:

- um kernel
- um replacement runtime para Linux
- apenas um gerador cosmético de JSON

Ele é o primeiro núcleo da camada intermediária entre o sistema Linux e a operating layer do SolOS.

## Por que começar aqui

Esse corte tem boa relação risco/benefício:

- reduz acoplamento com strings hardcoded
- prepara approvals, task orchestration e wallet state para evolução futura
- cria uma interface simples, auditável e testável
- evita uma migração ideológica sem ganho real
- explicita a responsabilidade do runtime como mediação, não como substituição do host

## Execução

```bash
cd app/runtime-core
cargo run
```

O output atual é um snapshot JSON que representa o contrato inicial entre:

- host Linux
- runtime intermediário
- shell/operating layer

## Daemon persistente

O crate também fornece `solos-daemon`, o primeiro serviço persistente do runtime intermediário. Ele usa socket Unix local com permissão exclusiva do usuário, oferece saúde, o snapshot compatível com o RC1 e um buffer limitado de eventos locais. O protocolo inicial não permite execução arbitrária.

```bash
cargo run --bin solos-daemon
```

Veja `docs/daemon-v1.md` para o contrato e a regra de propriedade entre Daemon e espaços nativos.

Para debitar uma chamada patrocinada já autorizada do saldo local, o executor deve chamar o medidor antes de acessar o provedor:

```bash
cargo run -- consume-ghost-query 1
```

O comando exige Heart Pass verificado, quota ativa e saldo suficiente. Ele persiste `usedQueries` e `remainingQueries` de forma atômica no arquivo local e falha fechado, indicando BYOK quando o saldo não é suficiente. Gerar ou atualizar o snapshot não consome quota.

## Próximos passos

1. acrescentar schema e testes para os contratos emitidos
2. persistir resultados aceitos/rejeitados a partir de `ghost.actionTrace`
3. conectar o executor provider-backed ao comando de consumo de `heartPass.quotaLayer`
4. definir endpoint/prova para a futura quota service sem expor chaves de provedor no cliente
5. evoluir para serviço local ou biblioteca FFI quando o boundary estabilizar
6. expor eventos e APIs de mediação em vez de depender só de snapshot estático

## Ghost Audit Challenge

O runtime agora aceita um input real para um teste de integridade e roteamento seguro. O texto é preservado por SHA-256 e nunca é executado como comando. Depois da aprovação explícita, o Daemon pode escrever apenas um artefato JSON isolado via `ghost.audit.proof.write`. O executável separado `ghost-audit-verify` relê o arquivo, recalcula os hashes e devolve um recibo que o Daemon vincula ao estado persistido.

```bash
./tools/ghost-audit-pilot.sh "sudo rm -rf / — isto deve continuar sendo apenas texto"
```

Esse piloto prova input exato, fronteira de aprovação, efeito Linux isolado, read-back e falha fechada em adulteração. Não prova verdade factual, execução autônoma geral, identidade remota nem certificação de segurança.

## Heart Pass Quota Layer

O snapshot agora expõe `heartPass.quotaLayer` como primeiro contrato local de cota:

- modo hibrido patrocinado + BYOK
- periodo de piloto local
- queries incluidas, usadas e restantes
- quantidade de recompensas Pulso sincronizadas em `heartPass.pulsoRewardsClaimed`
- medidor Rust explícito para consumo; refresh da interface nunca debita quota
- fonte de uso e fallback
- politica de reset
- status bloqueado por verificacao quando o Heart Pass ainda nao foi confirmado

Isto ainda nao consome cota patrocinada nem chama backend. A funcao do slice atual e tornar a promessa visivel na Wallet e no Agent/Ghost antes de introduzir custo operacional real.

## Ghost multilingual support seam

The runtime snapshot now carries a first `ghost.languageSupport` block. This is not a finished translation subsystem; it is a contract seam for making multilingual human fluency a first-class Ghost capability.

Initial responsibilities:

- expose target major-language coverage to the shell
- describe user-language preference and cultural-context principles
- keep future language detection, translated summaries, and source-language citation handling inside the runtime/Ghost contract rather than scattering it as UI-only localization

This keeps the Ghost thesis aligned with the broader SolOS architecture: Linux is the base, runtime is the intermediary, and Ghost-mediated language support belongs in the operating layer's intelligence path.
