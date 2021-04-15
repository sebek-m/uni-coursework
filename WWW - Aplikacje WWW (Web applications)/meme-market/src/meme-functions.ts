import { memeDB } from "./meme-db-instance";

export async function get_meme(id: number) {
  return await memeDB.getMeme(id);
}
