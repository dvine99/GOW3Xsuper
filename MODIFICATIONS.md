# Modificações mantidas pelo GOW3Xsuper

Este documento distingue as contribuições de **dvine99/GOW3Xsuper** da base de
**Devran Cosmo Uenal (Cosmo/gow3_xbox)**, derivada do **SpartanPort de Felipe Prado
e colaboradores**. Consulte os [créditos completos](xbox/CREDITS.md).

Base upstream incorporada: `fe8c66ed3a9bf151557c8f0af57009bdc955b042`
(`alpha-1.0.26299.2139`). Os commits e builds próprios estão no [histórico](CHANGELOG.md).

## Interface e identidade visual — desde 1.0.26300.308

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

## Preparação do primeiro início — 1.0.26300.436

Esta linha **retoma a .308** e reverte as alterações rejeitadas da .343.

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
