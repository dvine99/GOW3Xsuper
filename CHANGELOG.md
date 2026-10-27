# Histórico das versões próprias

As versões abaixo pertencem ao **GOW3Xsuper**. São prereleases experimentais do
fork de [Devran Cosmo Uenal](https://github.com/Cosmo/gow3_xbox), derivado do
[SpartanPort](https://github.com/FelipePrado0/SpartanPort).

## 1.0.26300.436 — 27/10/2026 — Retirada

Release e anexos excluídos a pedido do mantenedor. O registro abaixo descreve
trabalho histórico; estas mudanças não fazem parte da Beta 0. O histórico Git
foi preservado para rastreabilidade.

- Retorno à base própria **1.0.26300.308**, revertendo as mudanças da .343.
- Preparação automática de biblioteca ausente/incompatível no início normal.
- Pré-compilação do menu passa a preparar a biblioteca, conversores de upload
  de texturas e pipelines registrados.
- Início normal aguarda compilações necessárias em vez de omitir desenhos;
  início rápido preservado como alternativa com aviso.
- Verificação de integridade/prontidão do cache e selftests nativos ampliados.
- Mantidas interface trilíngue, capa/splash, controles e diagnóstico da .308.

Fonte: [`e640bfa5596cb4998e4433f07fb3a1de9ed96934`](https://github.com/dvine99/GOW3Xsuper/commit/e640bfa5596cb4998e4433f07fb3a1de9ed96934).
[Build Windows/UWP aprovada](https://github.com/dvine99/GOW3Xsuper/actions/runs/40449954030).
Pacote e dependência verificados; resultado visual e desempenho no console
**ainda não confirmados**. Novos shaders podem causar pausas.

## 1.0.26300.343 — 27/10/2026 — Retirada

Experimentos de readback, escalonamento de compilação e metadados de pacote.
O mantenedor relatou falha e solicitou desconsiderar esta versão. As alterações
foram revertidas na linha atual; **não há download recomendado desta versão**.
O histórico permanece no Git para rastreabilidade.

## Beta 0 — 1.0.26300.308 — 27/10/2026 — Versão publicada atual

A antiga release “base anterior” foi renomeada para **Beta 0**. O pacote, a
versão interna e o checksum permanecem os mesmos; não houve recompilação.

[Release e download](https://github.com/dvine99/GOW3Xsuper/releases/tag/alpha-1.0.26300.0308)

- Idiomas português brasileiro, espanhol e inglês para a interface do app.
- Nome de exibição God of War III, logo de abertura e capa em múltiplas escalas.
- Treze controles avançados para mecanismos existentes e persistência das opções.
- Coleta de métricas corrigida e analisador de logs de desempenho.
- Workflow de build, recursos indexados, assinatura e ZIP para teste no Xbox.
- Testes de recursos usam UTF-8 explicitamente também no Windows.

Fonte: [`ac0da17362ad5b0e50e0393d3e53b830ae54fc3c`](https://github.com/dvine99/GOW3Xsuper/commit/ac0da17362ad5b0e50e0393d3e53b830ae54fc3c).
[Build Windows/UWP aprovada](https://github.com/dvine99/GOW3Xsuper/actions/runs/40441138489).
É a versão escolhida pelo mantenedor como base para a retomada. Isso não constitui
benchmark controlado ou garantia de ausência de falhas.

## Base herdada

`Cosmo/gow3_xbox`, `alpha-1.0.26299.2139`, commit
`fe8c66ed3a9bf151557c8f0af57009bdc955b042`. Para os marcos anteriores, consulte o
[registro de desenvolvimento upstream](xbox/README.md) e os
[créditos completos](xbox/CREDITS.md).
