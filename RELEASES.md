# Releases do GOW3Xsuper

**[Abrir Releases e baixar as versões deste fork](https://github.com/dvine99/GOW3Xsuper/releases)**

| Versão | Tag | Uso |
| --- | --- | --- |
| **Beta 0 — 1.0.26300.308** | [alpha-1.0.26300.0308](https://github.com/dvine99/GOW3Xsuper/releases/tag/alpha-1.0.26300.0308) | Versão publicada atual; pacote original da .308. |

As versões .343 e .436 foram retiradas; a Release da .436 e seus anexos foram
excluídos. O histórico técnico permanece no Git e no relatório. Leia o
[histórico](CHANGELOG.md) antes de trocar de versão.

## Conteúdo dos downloads

- `GOW3xbox<versão>.zip`: aplicativo `.appx` e dependência VCLibs em `Dependencies`.
- Arquivo `.sha256` para conferir o ZIP da Beta 0.
- Não inclui dump, executável ou assets do jogo; o código-fonte é o da tag indicada.
- Pacote assinado com a chave pública de teste do projeto, para Developer Mode;
  não é um lançamento da Microsoft Store.

No PowerShell, use `Get-FileHash .\GOW3xbox<versão>.zip -Algorithm SHA256` e compare
com o arquivo `.sha256`. Instalação e requisitos estão no [README](README.md).

## Publicar as próximas versões

Para mantenedores com permissão no repositório:

1. Abra **Actions → Xbox build → Run workflow** e selecione a revisão desejada.
   O workflow deve estar incorporado ao branch padrão para aparecer no disparo
   manual; enquanto isso, os pushes ao branch de trabalho configurado geram artifacts.
2. Ative a opção de publicação de prerelease (`release`), preencha título e
   mudanças (`whats_new`, itens separados por `|`) e execute.
3. O workflow compila, testa, indexa recursos, empacota e assina. Se houver falha,
   corrija antes de publicar. A etapa de release anexa o ZIP aprovado.
4. Confira tag, commit, versão do manifesto e download. Registre as mudanças no
   [CHANGELOG](CHANGELOG.md) e atualize a tabela de versões do README.
5. Documente o resultado do teste no Xbox separadamente da build. Não anuncie
   correção visual ou ganho de FPS sem evidência do console.

O ZIP usa a versão real do app. A tag completa o último componente com zeros
para ordenação (`308` → `0308`); o manifesto e o ZIP continuam com `308`.
**Beta 0** é o nome público da Release; a tag técnica existente foi preservada.
Não sobrescreva uma versão já publicada com outro binário. Identifique versões
problemáticas como retiradas e publique uma versão nova com a correção.

## Origem e créditos de cada release

As notas devem identificar **GOW3Xsuper → gow3_xbox (Devran Cosmo Uenal) →
SpartanPort (Felipe Prado e colaboradores)**, linkar os créditos completos,
descrever as mudanças próprias, limitações e passos de instalação.
A Beta 0 usa exatamente o ZIP do build aprovado da .308 indicado no histórico;
a troca de nome não modifica o aplicativo nem constitui uma nova compilação.
