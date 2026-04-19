# SolOS Runtime Core

Primeiro embrião de subsistema em Rust para o SolOS.

## Propósito

Em vez de reescrever a shell nativa inteira, este crate define o primeiro boundary útil:

- Rust gera estado estruturado do runtime
- C++/Qt/QML consome esse estado para apresentar a shell
- a UI continua nativa e iterável
- a lógica sistêmica pode migrar por fronteiras claras

## Por que começar aqui

Esse corte tem boa relação risco/benefício:

- reduz acoplamento com strings hardcoded
- prepara approvals, task orchestration e wallet state para evolução futura
- cria uma interface simples, auditável e testável
- evita uma migração ideológica sem ganho real

## Execução

```bash
cd app/runtime-core
cargo run
```

O output atual é um snapshot JSON que representa o contrato inicial entre runtime e shell.

## Próximos passos

1. gerar snapshot em arquivo consumido automaticamente pela shell
2. substituir fixture estática por coleta real de estado
3. acrescentar schema e testes
4. evoluir para serviço local ou biblioteca FFI quando o boundary estabilizar
