import { MemeDB } from './meme-classes';

// W trakcie działania programu, __dirname będzie ściężką bezwzględną do katalogu build,
// więc, w połączeniu z '/../meme_db.db', położenie pliku z bazą danych nie jest uzależnione
// od miejsca, z którego wywołujemy skrypt www.
export const memeDB = new MemeDB(__dirname + '/../meme_db.db');