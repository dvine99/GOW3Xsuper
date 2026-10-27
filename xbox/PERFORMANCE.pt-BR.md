# Desempenho no Xbox — controles e diagnóstico do GOW3Xsuper

[English version](PERFORMANCE.md)

**Distribuição atual: Beta 0 (1.0.26300.308).** A Release .436 foi excluída.
As seções sobre preparação do primeiro início na .436 são histórico de um
experimento retirado e não descrevem o binário Beta 0. Veja o
[relatório completo](../MODIFICATIONS.md).

Base: `alpha-1.0.26299.2139`, commit
`fe8c66ed3a9bf151557c8f0af57009bdc955b042` de `Cosmo/gow3_xbox`.

Esta alteração integra controles de desempenho existentes ao menu e corrige a
coleta de métricas. Não há benchmark de gameplay desta alteração: o sandbox
Linux não tem Xbox, SDK do Windows, Dozen em execução nem dump/save do jogo.
Não foi medida uma melhora de FPS. As versões próprias .308 e .436 já têm
pacotes Windows/UWP compilados, assinados e verificados. Apenas a Beta 0 (.308)
continua disponível em [Releases](../RELEASES.md); a validação no console é separada.

## Menu

Durante o jogo, pressione **RS + LT** (ou **Insert**) e abra **Performance**.
A seção **Advanced performance - next start** contém:

| Opção | Padrão da Alpha 6 | Efeito |
| --- | --- | --- |
| Threaded command recording | On | Grava comandos Vulkan em outra thread. |
| Pipelined draw preparation | On | Sobrepõe preparação e gravação, preservando a ordem. |
| Memory copy workers | 2 | Usa 0, 1 ou 2 workers para cópias de buffers. |
| Persistent pipeline cache | On | Reutiliza pipelines entre execuções. Desativar não apaga arquivos. |
| Driver pipeline binaries | On | Reutiliza binários quando o driver oferece suporte. |
| Prepare graphics state in advance | On | Prepara estados de pipeline do Dozen antes dos draws. |
| Parallel shader cache loading | On | Carrega pipelines salvos com múltiplos workers antes do jogo. |
| Upload buffer memory | Cached | Alternativa Uncached para comparação de transferências. Não é streaming de modelos. |
| Log frame pacing and memory | Off | Tempos de frame, CPU e alocações GPU a cada cinco segundos. |
| Log CPU frame stalls | Off | Tempos de compilação, gravação e esperas. |
| Log resource loading | Off | Leitura de arquivos, cópias e paginação; liga também o log de frame pacing. |
| Log slow graphics commands | Off | Comandos cuja gravação na CPU leva pelo menos 10 ms. |
| Log CPU/GPU synchronization | Off | Esperas por semáforos e submissão; liga também o log de stalls. |

Feche o menu para salvar. Feche e reabra o aplicativo para aplicar essas opções.
O menu indica alterações pendentes. `LocalState/user/graphics.cfg` guarda as
escolhas, com chaves `xbox_*`. Valores inválidos voltam ao padrão daquela opção.
As configurações privadas de `graphics-debug.txt` têm precedência, inclusive
quando desativam um diagnóstico necessário a outro relatório. Um override com
valor fora das escolhas do menu (por exemplo, quatro workers) continua válido;
remova-o para controlar essa opção pelo menu.

Compilação assíncrona de novos shaders, frames em fila, resolução, limite de FPS,
readbacks diferidos e readbacks do frame anterior já estavam no menu. A opção de
**Precompile shaders** já existe no launcher e cobre apenas o subconjunto de
shaders suportado; não elimina toda compilação durante o jogo. Não há novos
padrões agressivos: as seleções de desempenho mantêm o perfil da Alpha 6 e os
novos diagnósticos ficam desligados por padrão.

## Correções e custo dos diagnósticos

- O log agora consulta sua ativação depois da leitura das configurações do Xbox.
  Antes, a variável global podia capturar a ausência de `SPARTAN_UWP_METRICS`
  durante a inicialização do processo.
- O caminho de apresentação chama o coletor de intervalos. Antes, `Presented()`
  não tinha chamadores e o log não recebia amostras de frame.
- Cada janela registra todos os intervalos: quantidade, soma, média, máximo,
  estimativas p95/p99 e quantos excedem 50 e 100 ms. Pausas maiores que um segundo
  não são descartadas. O histograma tem tamanho fixo, sem alocação por frame;
  percentis arredondam para cima em 1 ms. Se caírem na faixa de 2.000 ms ou mais,
  usa-se o máximo observado como limite superior.
- Contadores dos heaps do VMA substituem a varredura completa das alocações usada
  pelo relatório anterior. Isso reduz o trabalho do próprio profiling; não é
  evidência de ganho de FPS no jogo.
- `cpu=-1` significa amostra indisponível. CPU é uso do processo normalizado pelo
  número de threads lógicas. `gpu` e `reserved` são memória, não uso percentual
  ou tempo de execução da GPU. No Xbox, fazem parte da memória unificada: não
  devem ser somados à RAM do aplicativo.

## Coletar e comparar

1. Ative **Log frame pacing and memory** e **Log CPU frame stalls**. Para investigar
   esperas, ative também **Log CPU/GPU synchronization**. Use os logs de leitura
   e comandos lentos apenas quando necessários: instrumentação tem custo.
2. Reinicie, carregue o mesmo save e reproduza a mesma câmera, percurso e combate
   por um intervalo igual em cada configuração. Identifique separadamente
   primeira execução (cache frio) e execução seguinte (cache aquecido).
3. Baixe `loader.log` do aplicativo pelo Device Portal. Use os scripts existentes
   em `xbox/scripts` com a sua configuração local de dispositivo.
4. Execute, a partir da raiz do repositório:

   ```bash
   python3 scripts/analyze_xbox_performance.py /caminho/loader.log > desempenho.json
   ```

   O relatório calcula FPS pela soma de frames dividida pelo tempo medido,
   agrupa esperas por local e mostra picos de memória e contagem de frames lentos.
   Os percentis apresentados são os piores **por janela**; percentis de várias
   janelas não podem ser promediados para obter o percentil de toda a execução.
   Logs antigos sem os novos campos são contados, mas não usados para inventar
   uma medição. Sem amostras utilizáveis, o comando termina com código 1.
5. Logs incluem menus e carregamento. Para comparar gameplay, forneça um trecho
   delimitado da mesma cena nos dois logs, incluindo as janelas completas.
   Compare uma opção por vez. Para medir FPS final, repita com os diagnósticos
   detalhados desligados.

Esperas longas, compilação síncrona e custo de gravação são **indícios** para
investigação. Não provam isoladamente que a GPU está saturada. A documentação
original em `xbox/README.md` registra travamento em uma tentativa com timestamps
GPU; esta alteração não reativa essa técnica. Profiling real de execução da GPU
continua dependente de ferramentas compatíveis com o console/driver e de uma
reprodução no dispositivo.

## Cobertura do pedido

| Área | Situação nesta alteração |
| --- | --- |
| Profiling CPU/GPU | Coleta de CPU/frame pacing corrigida, logs de memória GPU e esperas expostos, analisador de logs adicionado. Execução real GPU pendente. |
| Cálculos redundantes de shaders | O recompilador já executa propagação de constantes e eliminação de código morto (`gpu/shadps4/shader_recompiler/recompiler.cpp`). Nenhuma nova transformação semântica foi introduzida. |
| Batching e instancing | Pendente: os draws traduzidos podem ter efeitos em memória e dependências de ordem. Falta identificar grupos realmente compatíveis. |
| Renderização multithread | Caminhos existentes de gravação, preparação e cópias expostos no menu. |
| Sincronização CPU/GPU | Opções existentes de readback mantidas; controle de gravação e diagnósticos de espera adicionados. Readbacks lineares obrigatórios continuam ativos. |
| Cache e pré-compilação | Cache persistente, binários do driver, estados Dozen e warm-up paralelo agora configuráveis. Pré-compilação do launcher preservada. |
| Compressão de texturas | Uploads BC1/BC3 e outros formatos do jogo já existem. Recompressão adicional requer preservar formato, aliases, render targets e readbacks; pendente. |
| Streaming de texturas/modelos | Paginação e coleta sob pressão existentes preservadas. Streaming progressivo de assets exige metadados e controle do carregador do jogo; pendente. |
| Memória e transferências | Controles de workers e memória de upload expostos; coleta de métricas passa a usar contadores de heaps. |
| Resolução dinâmica | Pendente. O Xbox seleciona patches de resolução na inicialização; mudar durante o jogo exige validar os targets e a preparação de memória. |
| FSR | Pendente. A versão Xbox força upscaler desligado e registra falha do shader FSR1. É necessário reparar e validar o caminho Dozen antes de oferecer uma opção funcional. |
| Iluminação e sombras | Pendente: identificar passes e validar equivalência visual. Os knobs de pesquisa não foram transformados em opções de qualidade. |
| Frustum/occlusion culling | Pendente: faltam limites espaciais e visibilidade confiáveis na camada de tradução. |
| LOD | Pendente: depende de modelos alternativos e seleção na lógica do jogo. |
| Pós-processamento | Pendente: mapear os passes e suas dependências antes de reduzir custo. |
| GPU-driven rendering | Pendente: requer redesenho do envio de comandos e validação das dependências dos draws traduzidos. |
| Camada de tradução | Controle da preparação antecipada de estados Dozen e diagnóstico de comandos lentos expostos. |
| Stuttering | Correção dos diagnósticos e acesso aos mecanismos existentes de compilação/cache; redução mensurada ainda pendente. |
| Largura de banda | Comparação entre buffers de upload com/sem cache e workers disponível; medição física de largura de banda pendente. |

## Verificação

Testes portáveis cobrem persistência, valores inválidos, precedência das
configurações privadas, dependências dos diagnósticos, aviso de reinicialização,
histograma, pausas longas e agregação dos logs. O teste das opções é compilado
com e sem `GOW3_UWP`; isso valida a lógica de configuração, não as APIs do Xbox.
As builds completas Windows/UWP e o empacotamento das versões .308/.436
passaram. A .436 também passou pelo selftest nativo de cache de shaders.
Navegação por controle, reprodução visual da falha e benchmarks no console
continuam sem verificação neste ambiente. As distribuições são alphas para teste.


## Primeiro início a partir da versão 1.0.26300.308

As alterações da versão1.0.26300.343 foram revertidas. No início normal, o app
prepara automaticamente uma biblioteca ausente/incompatível de shaders, aquece
10 conversores de envio de texturas e carrega os pipelines já registrados antes
de iniciar o jogo. A opção **Pré-compilar shaders** faz essas mesmas etapas no
menu; uma biblioteca válida e os pipelines já preparados são reutilizados.

Um desenho aguardando compilação não é descartado no modo **Iniciar jogo** do
Xbox. O caminho anterior podia perder desenhos únicos que escrevem em texturas
reutilizadas depois. Estados desconhecidos ainda precisam compilar ao serem
usados, causando espera; a preparação não conhece todos os estados do jogo.
O modo **Início rápido (pode omitir gráficos)** conserva a opção anterior de
compilar em segundo plano e descartar desenhos pendentes. A ajuda da opção de
compilação em segundo plano explica esse vínculo em português, espanhol e inglês.

Use **Iniciar jogo** para testar a primeira inicialização. O log identifica
`Xbox shader startup: complete`; os logs de preparação indicam as variantes
reutilizadas/compiladas e os pipelines carregados. Se a preparação de arquivos
falhar, o erro é registrado e o compilador normal continua sendo o fallback.
A confirmação da imagem enviada e dos tempos de carregamento exige teste real
no console. A foto isolada não comprova a causa de toda a corrupção observada.
