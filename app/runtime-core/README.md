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

## Próximos passos

1. gerar snapshot em arquivo consumido automaticamente pela shell
2. substituir fixture estática por coleta e mediação real de estado
3. acrescentar schema e testes
4. evoluir para serviço local ou biblioteca FFI quando o boundary estabilizar
5. expor eventos e APIs de mediação em vez de depender só de snapshot estático

## Ghost multilingual support seam

The runtime snapshot now carries a first `ghost.languageSupport` block. This is not a finished translation subsystem; it is a contract seam for making multilingual human fluency a first-class Ghost capability.

Initial responsibilities:

- expose target major-language coverage to the shell
- describe user-language preference and cultural-context principles
- keep future language detection, translated summaries, and source-language citation handling inside the runtime/Ghost contract rather than scattering it as UI-only localization

This keeps the Ghost thesis aligned with the broader SolOS architecture: Linux is the base, runtime is the intermediary, and Ghost-mediated language support belongs in the operating layer's intelligence path.
