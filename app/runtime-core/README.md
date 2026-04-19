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
