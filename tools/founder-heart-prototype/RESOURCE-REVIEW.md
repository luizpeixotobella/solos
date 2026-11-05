# Revisão de recursos e dependência — 05/11/2026

Solicitação: explicar a escolha de Safe versus carteira própria e evitar uma
expansão que exceda recursos, manutenção ou orçamento de contexto do projeto.

## Decisão revista

**Manter Safe como candidata de laboratório, não incluir o laboratório na ISO
padrão.** Continuar com a Wallet Qt e créditos no CMS; integração on-chain é
opcional e tem gate separado. Nenhum serviço, mainnet, compra ou build foi iniciado.

Reutilizar a conta Safe foi uma decisão de reduzir a superfície criptográfica
própria: a validação de assinaturas, nonce e execução vem de contratos existentes.
Criar equivalente do zero transfere também a revisão/auditoria, recuperação,
compatibilidade e manutenção para nossa equipe. Não basta conseguir compilar.
O contrato personalizado de benefícios continua nosso e não foi auditado para
produção; usar Safe não torna automaticamente segura toda a integração.

## Tipos diferentes de dependência

- **Código:** Safe é código aberto LGPL-3.0; a cópia usada no laboratório está
  fixada em1.5.0 e mantém sua licença. Ainda há dependência técnica e obrigações
  de distribuição; não significa autoria própria ou ausência de risco.
- **Serviço hospedado:** o laboratório não faz chamadas à API/servidores Safe,
  não tem conta Safe e não usa bundler, paymaster ou transaction service remoto.
  Não há assinatura mensal contratada.
- **Rede pública:** uma futura Polygon exigiria RPC, gás, monitoramento e
  recuperação. Isso não está implementado nem tem custo de operação estimado.
- **Custódia:** as chaves atuais são aleatórias e efêmeras do laboratório, não
  uma carteira pessoal do apoiador. UX de recuperação/passkeys permanece pendente.

## Peso real e correção de rumo

`node_modules/` do laboratório mediu aproximadamente167MiB no Gateway, principalmente
por EVM/compiler/tooling. Não é medição de RAM da Wallet, tamanho do contrato
on-chain ou incremento comprimido exato da ISO. O protótipo inteiro seria um pacote
de desenvolvimento excessivo como dependência obrigatória da experiência inicial.

O overlay preparado em04/11 continua artefato opcional, **não deve ser incorporado
à próxima ISO por padrão**. Nenhuma ISO foi reconstruída ou alterada. Se quisermos
mostrar a experiência no sistema, começar por uma prévia leve e explicitamente
demonstrativa; não embutir Hardhat/solc ou chaves preexistentes na versão normal.

## Sequência compatível com recursos

1. Manter cadastro, benefícios e extrato em reais/créditos funcionais sem cripto.
2. Validar demanda e limites de uso com o piloto; manter registro técnico curto.
3. Somente se necessário, construir um adaptador opcional de verificação/assinatura,
   reaproveitando a interface Qt e evitando vínculo obrigatório a um SaaS.
4. Estimar RPC/gás/suporte, recuperação e revisão de segurança antes de rede pública.
5. Considerar carteira própria apenas com requisitos que as opções existentes não
   atendam e orçamento explícito para manter e auditar esse componente.

Para poupar contexto e execução: decisões em documento pequeno, tarefas delimitadas,
reutilização de evidência já medida e nenhuma rodada repetida de treino/inferência
sem necessidade. Um protótipo aprovado não equivale a compromisso de produto.

Referências oficiais: https://docs.safe.global/advanced/smart-account-overview e
licença da versão instalada em `node_modules/@safe-global/safe-smart-account/LICENSE`.
