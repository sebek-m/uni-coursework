/*
  Tworzy napis z liczbą sekund i trzema miejscami po przecinku, odpowiadającymi milisekundom
*/
export function msToSecString(ms: number): string {
  return (ms / 1000).toFixed(3) + "s";
}