# Relatório completo de modificações — GOW3Xsuper

Este documento distingue as contribuições de **dvine99/GOW3Xsuper** da base de
**Devran Cosmo Uenal (Cosmo/gow3_xbox)**, derivada do **SpartanPort de Felipe Prado
e colaboradores**. Consulte os [créditos completos](xbox/CREDITS.md).

Base upstream incorporada: `fe8c66ed3a9bf151557c8f0af57009bdc955b042`
(`alpha-1.0.26299.2139`). Os commits e builds próprios estão no [histórico](CHANGELOG.md).

## Situação da distribuição — 27/10/2026

A única Release publicada é **Beta 0**, nome público do pacote **1.0.26300.308**.
O nome, a versão interna, a assinatura e o SHA256 do binário são independentes:
a renomeação da Release não recompila nem altera o ZIP.

| Linha | Situação | Conteúdo |
| --- | --- | --- |
| Beta 0 / .308 | Publicada | Interface, identidade visual, controles, profiling e distribuição descritos abaixo. |
| .343 | Retirada e mudanças revertidas na linha posterior | Experimentos de readback e escalonamento; não integra Beta 0. |
| .436 | Release excluída | Experimentos de preparação do primeiro início; não integra Beta 0. |

A exclusão de uma Release remove sua página de distribuição e anexos, sem
reescrever o histórico Git. O branch de desenvolvimento mantém os commits das
experiências; para reproduzir a Beta 0, use o commit/tag exato da .308.

## Interface e identidade visual — Beta 0 / 1.0.26300.308

- Seleção de **pt-BR, es e en** no menu inicial e nas configurações, aplicada aos
  menus, ajuda, mensagens, diálogos e telas controladas pelo aplicativo.
- Preferência persistida em `graphics.cfg`, carregada antes dos diálogos iniciais;
  integração com o idioma do seletor de arquivos nativo.
- Catálogo central em [interface.json](xbox/locales/interface.json), geração de
  cabeçalhos e testes de cobertura/paridade. Não altera a localização dos assets
  ou o áudio do jogo.
- Nome de exibição **God of War III** no manifesto e interface. A identidade
  interna `GodOfWar3Launcher`, o publisher e o nome técnico do `.appx` são mantidos
  para compatibilidade de atualização; não são o título de exibição.
- Logo fornecido para o splash e capa fornecida com imagens quadradas e largas,
  variantes de escala 100/200/400 e indexação MakePRI no pacote único.
  A fonte de capa de alta resolução é preservada no repositório; os tiles são
  gerados por [generate_xbox_artwork.py](scripts/generate_xbox_artwork.py).
  A seleção e o cache de imagens pela interface do Xbox exigem verificação real.

## Controles avançados — desde 1.0.26300.308

A integração ao menu, persistência, validação de valores e indicação de reinício
são deste fork. **Os mecanismos gráficos controlados já existiam na base.**

| Controle | Função |
| --- | --- |
| Threaded command recording | Gravação de comandos em outra thread. |
| Pipelined draw preparation | Sobreposição da preparação e gravação, preservando ordem. |
| Memory copy workers | Seleção de zero, um ou dois workers de cópia. |
| Persistent pipeline cache | Reutilização do cache entre execuções. |
| Driver pipeline binaries | Reutilização de binários quando suportada pelo driver. |
| Prepare graphics state in advance | Preparação antecipada de estados Dozen. |
| Parallel shader cache loading | Aquecimento paralelo de pipelines salvos. |
| Upload buffer memory | Comparação entre memória com cache e sem cache. |
| Log frame pacing and memory | Intervalos de apresentação, CPU e memória. |
| Log CPU frame stalls | Compilação, gravação e esperas na CPU. |
| Log resource loading | Leituras, cópias e paginação. |
| Log slow graphics commands | Gravação de comandos lentos na CPU. |
| Log CPU/GPU synchronization | Esperas de submissão e semáforos na CPU. |

Implementação: [spartanport_xbox_performance.h](gpu/shim/spartanport_xbox_performance.h).
Valores padrão e coleta: [guia de desempenho](xbox/PERFORMANCE.pt-BR.md).
Overrides privados continuam tendo precedência; diagnósticos ficam desligados
por padrão e adicionam custo quando ativados.

## Profiling — desde 1.0.26300.308

- Ativação da coleta após leitura das configurações e ligação do coletor ao
  caminho de apresentação.
- Histograma sem alocações por frame, média, máximo, estimativas p95/p99 e
  contagem de intervalos acima de 50/100 ms, incluindo pausas longas.
- Contadores dos heaps VMA em lugar da varredura de todas as alocações no relatório.
- [Analisador de logs](scripts/analyze_xbox_performance.py) com agregação de FPS,
  esperas e memória, e testes de configuração, histogramas e agregação.

Memória GPU não é utilização ou tempo de execução da GPU. Os diagnósticos de
espera medem o lado CPU; não reativamos timestamps GPU nem afirmamos ganho de FPS.

## Experimento retirado: preparação do primeiro início — 1.0.26300.436

Esta linha histórica **retoma a .308** e reverte as alterações rejeitadas da .343.
Sua Release foi excluída a pedido do mantenedor; as mudanças abaixo **não
integram a Beta 0**.

- Verificação de índices de shaders de vértice e fragmento, perfil compatível e
  tamanho real dos arquivos de cache antes de considerar a biblioteca pronta.
- Início normal tenta preparar automaticamente a biblioteca ausente/incompatível.
- **Pré-compilar shaders** prepara a biblioteca, aquece dez conversores comuns de
  upload de texturas (1D/2D, 8/16/32/64/128 bits) e pipelines do jogo já registrados.
- Aquecimento dos conversores no máximo uma vez por execução do app; início
  normal aguarda o aquecimento dos pipelines registrados antes do guest.
- No início normal do Xbox, um desenho que depende de compilação aguarda o shader
  ou pipeline. Evita descartar esse desenho, que pode escrever numa textura usada
  depois. O modo rápido mantém o comportamento assíncrono anterior explicitamente.
- Ajuda atualizada nos três idiomas e selftest nativo para caches ausentes,
  incompletos, incompatíveis ou truncados, além da validação já existente.

Isso prepara **conversores e pipelines**, não texturas/modelos que ainda não foram
solicitados pelo jogo. Estados desconhecidos continuam exigindo compilação ao
primeiro uso. Há uma troca entre completude dos desenhos e pausas; a correção da
imagem relatada e os tempos reais ainda dependem de teste no console.

## Build e distribuição

O selftest nativo ampliado de shaders entrou na linha .436; a Beta 0 conserva
os testes e a configuração de build do seu commit original.


- [Workflow Xbox](.github/workflows/xbox.yml) com build Dozen em Windows/VS2022 e
  aplicativo em Windows/VS2026, dependências fixadas/cacheadas e checksum do DXC.
- Testes de traduções/arte, geração de recursos, selftest de shaders, compilação
  nativa, MakePRI, MakeAppx e assinatura de teste antes de disponibilizar o pacote.
- ZIP **GOW3xbox<versão do app>.zip**, incluindo `.appx` e VCLibs em `Dependencies`.
- Publicação opcional de prerelease por execução manual e documentação de
  origem/instalação. [Política e versões](RELEASES.md).

## Recursos herdados e trabalhos não implementados

Loader/runtime, tradução gráfica shadPS4, Dozen, paginação, patches do jogo,
áudio/vídeo, suporte a controle e caminhos existentes de cache, readback e
multithreading vêm dos projetos anteriores. Expor uma opção não significa
reescrever esses subsistemas.

Não implementamos novos sistemas de batching/instancing, GPU-driven rendering,
LOD/culling, streaming progressivo de assets, recompressão de texturas, resolução
dinâmica, FSR funcional no Xbox ou novos passes de iluminação/pós-processamento.
As propostas inviáveis foram deixadas fora, conforme orientação do mantenedor.

## Alterações retiradas

A **1.0.26300.343** experimentou mudanças de coerência de readback, escalonamento
justo de compilação, lotes menores de warm-up e metadados/nome técnico de pacote.
Após o relato de falha, essas mudanças foram **revertidas**, preservando o
histórico Git. Não integram a .436; não são apresentadas como melhorias mantidas.

## Registro de implementação e manutenção do repositório

| Commit | Trabalho realizado | Situação |
| --- | --- | --- |
| `c0ee8496c3a905888e7e4b5eed453614f994f9af` | Controles, métricas, localização, imagens e workflow de distribuição. | Integra Beta 0. |
| `ac0da17362ad5b0e50e0393d3e53b830ae54fc3c` | Leitura UTF-8 nos testes Windows; fonte exata do pacote .308. | Beta 0 publicada. |
| `e47a17c89c9df50ed045d338b5ad8d48b6092612` | Readback, fila de compilação, lotes e metadados de nome. | Experimento .343 revertido. |
| `4918de942f16e85e43d437bb250072db9bfb6152` | Log da identidade instalada após abertura do loader.log. | Experimento .343 revertido. |
| `444c4e83cfd87de0d0d9020dc070daba1f02a41a` | Reversão da .343 e preparação de biblioteca/conversores/pipelines. | Linha .436 retirada da distribuição. |
| `e640bfa5596cb4998e4433f07fb3a1de9ed96934` | Limpeza completa do diretório temporário no selftest de shaders. | Fonte da .436 retirada. |
| `a5206298c95c9c19ea42d29416b37371ef99465d` | README em português, cadeia de forks, créditos, inventário, changelog, guia de Releases e modelo de notas do workflow. | Documentação do repositório. |

### Publicação e documentação

- Créditos explícitos a Devran Cosmo Uenal, Felipe Prado/SpartanPort e demais
  componentes, preservando a licença e os avisos de terceiros.
- Separação entre contribuições próprias e herdadas, experimentos retirados e
  propostas não implementadas.
- Publicação inicial dos ZIPs originais .308 e .436, com notas em português,
  commits de origem, dependência e checksums; posterior exclusão da Release .436.
- Renomeação da Release .308 de “base anterior” para **Beta 0**.
- Atualização de README, histórico e guia de Releases para indicar somente Beta 0
  como versão publicada atual; manutenção deste arquivo como relatório completo.
- As alterações de documentação estão no PR #1. A atualização dos campos About
  do GitHub foi tentada, mas recusada com HTTP 403; esses campos não foram alterados.

### Verificações e limites

A Beta 0 passou pela compilação Windows/UWP, testes de recursos, MakePRI,
MakeAppx, assinatura e verificação de integridade de pacote/dependência. A .436
teve também selftest nativo de cache aprovado antes de sua retirada. Esses
resultados pertencem aos respectivos commits e não comprovam correção visual,
instalação ou desempenho no console. Não há benchmark controlado deste fork.

SHA256 do ZIP Beta 0:
`1668730b0903ea4c2115d97e4d6f15135f4928f5e80feb149f4284e0028d94aa`.

Os detalhes de versão e links de build permanecem no [CHANGELOG](CHANGELOG.md).
