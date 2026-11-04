# SolOS Heart Pass Lab — proposta para a próxima ISO

**Protótipo isolado, não oferta comercial nem carteira pessoal.** Nenhum termo,
pagamento, crédito ou contrato da campanha em produção é alterado.

## Recomendação de produto

- Manter Apoiador e Founder Heart com os créditos existentes.
- Propor Patrono com 250 créditos **mais um passe inteiro de acesso**, não uma
  fração do NFT artístico. O passe não representa sociedade ou participação em receita.
- Criar contrato de utilidade separado: a obra Polygon existente continua intacta.
- Tornar esse passe pessoal e não transferível; a recuperação futura deve recuperar
  o controle da mesma smart account, sem comercializar a identidade do fundador.
- Comprar cripto não deve ser pré-requisito para apoiar. O pagamento confirma a
  elegibilidade; o titular escolhe vincular sua carteira e assinar o resgate.
- Manter créditos, custos, limites de consulta e dados de pagamento fora da blockchain.
  On-chain ficam somente referências opacas não pessoais e eventos de acesso.
- Preservar quem já possui a obra/Heart Pass existente por uma política de compatibilidade
  explícita; não substituir o contrato artístico nem retirar utilidade concedida.

Essa proposta **não foi publicada nos termos**. Preço final, estoque, orçamento por
usuário, recuperação, consentimento e política de revogação precisam estar coerentes
antes de oferecer emissão real. R$500 continua sendo o mínimo publicado do Patrono,
não um teto adotado por este laboratório.

## O que funciona aqui

1. Instancia uma EVM Hardhat/EDR **em memória**, chain31337, sem RPC HTTP.
2. Cria chaves aleatórias temporárias e implanta **Safe1.5.0 + ProxyFactory +
   CompatibilityFallbackHandler** dos artefatos oficiais.
3. Implanta um ERC-1155 experimental com base em OpenZeppelin5.6.1.
4. Simula apoio confirmado: registra créditos fictícios e atesta elegibilidade.
5. Assina uma transação Safe com a chave efêmera do laboratório; o contrato valida
   a assinatura e entrega um passe à Safe. O usuário da tela **não está assinando
   com uma carteira própria**: a tela diz explicitamente “simular assinatura”.
6. Rejeita emissor/assinatura/carteira errados, duplicidade, transferência do passe
   e repetição de assinatura. Múltiplos apoios não duplicam o passe pessoal.
7. Estorno fictício revoga a elegibilidade correspondente; o passe só é queimado
   quando não resta outro apoio ativo. Os créditos, ainda não consumíveis no lab,
   são revertidos. Não há estorno bancário real.

**Não implementado:** Stripe/PIX real, prova de pagamento verificável on-chain,
produção/testnet, titular autenticado no CMS, dispositivo seguro, passkeys,
recuperação, bundler/paymaster ERC-4337 ou pesquisa patrocinada. O emissor do passe
tem poder de atestar e revogar; isso precisa ser limitado e auditado na produção.
O mecanismo demonstrado é Safe `execTransaction`, não execução ERC-4337.

## Executar

Node22.13+ em versão suportada pelo Hardhat (testado com Node24.22.1). Ambiente
Linuxx64 testado; dependências nativas EDR precisam corresponder ao alvo da ISO.

```sh
cd tools/founder-heart-prototype
npm ci --ignore-scripts --no-fund
npm test
npm run demo
npm start
```

Abrir `http://127.0.0.1:8766`. O servidor só escuta loopback; Host, Origin e token
de sessão são verificados nas ações. Nenhum endpoint genérico de assinatura/RPC
fica exposto. Encerre com Ctrl+C. A blockchain e chaves desaparecem. Para testar
outro nível na tela, reinicie o servidor. `artifacts/demo.json` registra somente
endereços, hashes e estados fictícios; nenhuma chave privada.

## Próxima ISO: instalação opcional, não automática

O protótipo é uma aplicação separada. Não troca a Wallet atual, não altera o
autostart e **não contém a correção QML39e4072**, que deve continuar na base da ISO.
Não basta portar a branch deste protótipo como se fosse a base já testada.

Após `npm ci` no Linuxx64 de build, prepare um overlay novo:

```sh
python3 tools/founder-heart-prototype/scripts/stage-iso-overlay.py /tmp/solos-heart-lab-overlay
```

O overlay contém `/opt/solos/labs/heart-pass`, um launcher em `/usr/local/bin` e
um atalho “Heart Pass Lab” em `/usr/share/applications`. Não tem serviços,
autostart, carteiras pré-criadas, banco persistente ou segredos. Use uma revisão
do build Ubuntu JI com Node compatível, navegador, `xdg-open` e dependências EDR.
Importe o overlay pela etapa de includes **apenas na próxima build isolada**.
O script não instala no host, não modifica configs e recusa destino já existente.

Antes de declarar pronto na ISO: boot em VM, abrir atalho, simular apoio/resgate/
estorno, capturar tela, reiniciar lab e comprovar estado zerado. O teste na VM e
a geração de nova ISO **ainda não foram executados** nesta entrega.

## Antes de rede pública

- Substituir atestação fictícia por contribuição liquidada, reconciliada e
  idempotente, com fila transacional/outbox e tratamento de eventos fora de ordem.
- Autenticar conta CMS e provar controle da Safe por desafio e assinatura
  compatível com contratos (ERC-1271), não apenas cadastrar endereço.
- Guardar a autoridade emissora fora da ISO, com controle de acesso; não enviar
  emails, valores ou referências de provedor aos eventos públicos.
- Definir recuperação testada antes de vender a experiência como não custodial;
  não criar chaves de cliente no CMS. Passkeys não são habilitadas neste lab.
- Auditar o contrato personalizado e testar revogação, substituição de proprietário,
  contingência do emissor, privacidade e conciliação antes de mainnet.
- Integrar os passes novo e antigo explicitamente na Wallet/runtime. O contrato
  novo deste lab **não é reconhecido** pelo verificador Polygon atual.

## Escolha de componentes e referências

Não se criou uma carteira criptográfica do zero: Safe mantém a conta inteligente;
OpenZeppelin implementa ERC-1155 e controle de propriedade; Hardhat/EDR simula a EVM.
O contrato de benefícios é código experimental próprio, não auditado por esses projetos.

- https://docs.safe.global/advanced/smart-account-overview
- https://docs.openzeppelin.com/contracts/5.x/erc1155
- https://hardhat.org/docs/reference/edr-simulated-networks
- https://eips.ethereum.org/EIPS/eip-4337

Versões fixadas em package-lock. Overrides tmp0.2.7 e zod4.6.7 corrigem alertas
transitivos detectados no preflight. Ganache foi avaliado e substituído por EDR;
não é dependência desta entrega. Não usar `npm audit fix --force` no build.
