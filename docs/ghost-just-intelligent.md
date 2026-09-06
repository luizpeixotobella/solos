# Ghost Just Intelligent (JI)

## A nova máxima do SolOS

O SolOS preserva a intuição de **Just in Time (JIT)** — fazer a coisa certa no momento certo — e acrescenta uma camada de comportamento:

> **Just Intelligent (JI): inteligência mimetizada no fluxo, na medida certa e no momento certo.**

“Mimetizada” não significa inteligência falsa. Significa que Ghost não precisa teatralizar uma personalidade para demonstrar capacidade. A inteligência aparece como uma escolha adequada de contexto, ferramenta, tom, permissão, evidência e próximo passo.

## O governador de autonomia

O `runtime-core` agora projeta `ghost.autonomyGovernor` a partir de metadados do ledger durável. O governador lê somente contagens e estados operacionais minimizados:

1. **observe-local** — lê fatos locais, documentação e eventos permitidos;
2. **propose-route** — classifica intenção, explica a rota e prepara um plano;
3. **approval-bound-action** — executa uma capability estreita somente depois de aprovação explícita;
4. **verify-and-learn** — verifica o resultado e separa feedback humano de log operacional.

Outcomes verificados e traces fazem o Ghost subir de `observe` para `propose-with-evidence` e, depois, para `approval-bound`. Essa progressão é deliberadamente conservadora: nenhuma quantidade de eventos transforma escrita, rede, wallet, shell ou publicação em ação autônoma sem aprovação.

## O que é autonomia aqui

Autonomia, nesta etapa, é Ghost conseguir:

- escolher uma rota local quando a evidência já existe;
- evitar gasto de pesquisa quando o cache basta;
- reconhecer que um pedido exige approval;
- montar a sequência objetivo → plano → approval → capability → verificação → evidência;
- preservar negações e falhas como sinais para não repetir uma rota insegura;
- recomendar o próximo gate com base no que foi realmente observado.

Não é autonomia irrestrita, execução arbitrária, aprendizado silencioso nem autorização implícita para publicar, pagar ou alterar o host.

## Relação com `data + results = algorithms`

O perceptron oferece a imagem didática: entradas recebem pesos, passam por uma soma e produzem uma decisão. O Ghost aplica a mesma intuição em camadas de sistema:

```text
dados locais + evidência + resultado observado
        -> rota explicável
        -> aprovação adequada
        -> efeito mediado
        -> verificação
        -> próximo comportamento mais bem calibrado
```

O ledger é evidência de execução, não aprendizado automático. Para uma mudança de comportamento virar aprendizado confiável, o resultado precisa ser avaliado em uma camada própria, com feedback humano aceito, rejeitado ou corrigido, versão, hipótese e rollback.

## Próximo gate

O próximo avanço é provider-backed e controlado: coletar outcomes humanos suficientes, definir uma capability estreita, aplicar quota/idempotência/expiração, assinar a política e provar rollback. Até esse gate, JI significa **mais contexto e melhor proposta**, não mais poder oculto.

## Autoria e responsabilidade

Este corte foi desenvolvido por **Luigi, inteligência artificial da LBArtes**, sob direção humana de Luiz. A IA participa de pesquisa, arquitetura, codificação, testes, documentação e recomendações; Luiz permanece fundador, financiador, diretor humano e decisor final.
