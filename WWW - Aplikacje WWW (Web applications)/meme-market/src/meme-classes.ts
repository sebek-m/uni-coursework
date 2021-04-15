import * as sqlite3 from 'sqlite3';

export class User {
  private memId: number;
  private memUsername: string;
  private memPassword: string;

  constructor(id: number, username: string, password: string) {
    this.memId = id;
    this.memUsername = username;
    this.memPassword = password;
  }

  get id(): number {
    return this.memId;
  }

  get username(): string {
    return this.memUsername;
  }

  public validPassword(password: string): boolean {
    return this.memPassword === password;
  }
}

function zeroPaddedStr(str: string): string {
  return str.length < 2 ? '0' + str : str;
}

export class DatedPrice {
  private memPrice: number;
  private memDate: Date;

  constructor(price: number, date?: Date) {
    this.memPrice = price;
    this.memDate = date ? date : new Date();
  }

  get price(): number {
    return this.memPrice;
  }

  get date(): string {
    let day: string = zeroPaddedStr(String(this.memDate.getDate()));
    let month: string = zeroPaddedStr(String(this.memDate.getMonth() + 1));
    let year: string = String(this.memDate.getFullYear());

    return year + '-' + month + '-' + day;
  }

  get dateTime(): string {
    let hours: string = zeroPaddedStr(String(this.memDate.getHours()));
    let minutes: string = zeroPaddedStr(String(this.memDate.getMinutes()));
    let seconds: string = zeroPaddedStr(String(this.memDate.getSeconds()));

    return `${this.date} ${hours}:${minutes}:${seconds}`;
  }
}

export class PriceHistory {
  private memPrices: DatedPrice[];

  constructor(prices?: DatedPrice[]) {
    this.memPrices = prices ? prices : [];
  }

  /*
    Dodaje cenę z datą na początku historii cen.
  */
  public addPrice(price: number): DatedPrice {
    let newPrice = new DatedPrice(price);

    this.memPrices.unshift(newPrice);

    return newPrice;
  }

  public getLatestPrice(): number {
    return this.memPrices[0].price;
  }

  public isEmpty(): boolean {
    return this.memPrices.length == 0;
  }

  get prices(): DatedPrice[] {
    return this.memPrices;
  }
}

interface MemeOptions {
  author?: User;
  priceHistory?: PriceHistory;
  id?: number;
  db?: MemeDB;
}

export class Meme {
  private memID: number | null;
  private memName: string;
  private memUrl: string;
  private priceHistory: PriceHistory;

  private myDB?: MemeDB;

  /*
    Zakładamy, że jeśli ktoś poda priceHistory, to jest ona posortowana od najnowszej do najstarszej
    oraz nie zawiera DatedPrice z datą późniejszą niż obecna.
  */
  constructor(name: string, url: string, price: number, options?: MemeOptions) {
    this.memName = name;
    this.memUrl = url;

    if (options) {
      this.memID = options.id ? options.id : null;
      this.priceHistory = options.priceHistory ? options.priceHistory : new PriceHistory();

      if (options.db)
        this.myDB = options.db;

        this.change_price(price, options.author);
    }
    else {
      this.memID = null;
      this.priceHistory = new PriceHistory();
      this.change_price(price);
    }
  }

  get price(): number {
    return this.priceHistory.getLatestPrice();
  }

  public async change_price(price: number, author?: User) {
    // Zmieniamy cenę tylko jeśli jest
    // rzeczywiście inna od aktualnej ceny.
    if (this.priceHistory.isEmpty() || price != this.price) {
      let newPrice = this.priceHistory.addPrice(price);

      if (this.myDB && author)
        await this.myDB.addMemePrice(this.id, newPrice, author);
    }
  }

  get id(): number {
    return this.memID!;
  }

  get name(): string {
    return this.memName;
  }

  get url(): string {
    return this.memUrl;
  }

  /*
    Zwraca tablicę reprezentującą historię cen.
  */
  get prices(): DatedPrice[] {
    return this.priceHistory.prices;
  }
}

export class MemeDB {
  private dbName: string;

  constructor(name: string) {
    this.dbName = name;
  }

  public async initDB() {
    sqlite3.verbose();
    let db = new sqlite3.Database(this.dbName);

    let creationProm = new Promise<MemeDB>((resolve, reject) => {
      db.serialize(() => {
        db.run('DROP TABLE IF EXISTS price;')
          .run('DROP TABLE IF EXISTS meme;')
          .run('DROP TABLE IF EXISTS user;')
          .run(`CREATE TABLE IF NOT EXISTS user (id INTEGER PRIMARY KEY,
                                                 username TEXT NOT NULL UNIQUE,
                                                 password TEXT NOT NULL);`)
          .run(`CREATE TABLE IF NOT EXISTS meme (id INTEGER PRIMARY KEY,
                                                 name TEXT NOT NULL,
                                                 url TEXT NOT NULL);`)
          .run(`CREATE TABLE IF NOT EXISTS price (meme_id INTEGER NOT NULL,
                                                  date TEXT NOT NULL,
                                                  price INTEGER NOT NULL,
                                                  author_id INTEGER NOT NULL,
                                                  FOREIGN KEY(meme_id) REFERENCES meme(id),
                                                  FOREIGN KEY(author_id) REFERENCES user(id),
                                                  PRIMARY KEY(meme_id, date, price));`, [], (err) => {
          if (err)
            return reject(err.message);
          
          resolve();
        });
      });
    });

    await creationProm;

    db.close();
  }

  public async getUser(identifier: string | number, colName: string): Promise<User | false> {
    sqlite3.verbose();
    let db = new sqlite3.Database(this.dbName);

    let queryProm = new Promise<User | false>((resolve, reject) => {
      db.get(`SELECT * FROM user WHERE ${colName} = ?;`, identifier, (err, row) => {
        if (err)
          return reject(`${err.message}, Błąd w trakcie wyszukiwania użytkownika`);

        if (!row)
          return resolve(false);

        resolve(new User(row.id, row.username, row.password));
      });
    });

    db.close();

    return await queryProm;
  }

  public async addUser(username: string, password: string): Promise<User> {
    sqlite3.verbose();
    let db = new sqlite3.Database(this.dbName);

    let creationProm = new Promise<User>((resolve, reject) => {
      db.run('INSERT OR IGNORE INTO user (username, password) VALUES (?, ?);', [username, password], function(err) {
        if (err)
          return reject(`${err.message}, Błąd w trakcie dodawania użytkownika`);

        resolve(new User(this.lastID, username, password));
      });
    });

    db.close();

    return await creationProm;
  }

  public async addMeme(meme: Meme, author: User, db?: sqlite3.Database) {
    let dbLoc: sqlite3.Database;

    if (db) {
      dbLoc = db;
    }
    else {
      sqlite3.verbose();
      dbLoc = new sqlite3.Database(this.dbName);
    }

    let placeholders = meme.prices.map((price) => '(?, ?, ?, ?)').join(',');
    let sql = 'INSERT OR IGNORE INTO price (meme_id, date, price, author_id) VALUES ' + placeholders;

    let addProm = new Promise<(string | number)[][]>((resolve, reject) => {
      if (meme.id) {
        dbLoc.run('INSERT OR IGNORE INTO meme (id, name, url) VALUES (?, ?, ?);', [meme.id, meme.name, meme.url], (err) => {
          if (err)
            reject(err);
        });

        resolve(meme.prices.map((price) => [meme.id, price.dateTime, price.price, author.id]));
      }
      else {
        // function zamiast notacji strzałkowej, żeby móc użyć this.lastID
        dbLoc.run('INSERT INTO meme (name, url) VALUES (?, ?);', [meme.name, meme.url], function(err) {
          if (err)
            return reject(err);

          resolve(meme.prices.map((price) => [this.lastID, price.dateTime, price.price, author.id]));
        });
      }
    });

    let prices = await addProm;

    let addPricesProm = new Promise((resolve, reject) => {
      dbLoc.run(sql, prices.flat(), (err) => {
        if (err)
          reject(err);

        resolve();
      });
    });

    await addPricesProm;

    console.log(`Added meme: ${meme.name}`);

    if (!db)
      dbLoc.close();
  }

  public async addMemes(memes: Meme[], author: User) {
    sqlite3.verbose();
    let db = new sqlite3.Database(this.dbName);
  
    for (const meme of memes) {
      await this.addMeme(meme, author, db);
    }
  
    db.close();
  }

  public async getThreeMostExpensive(): Promise<Meme[]> {
    sqlite3.verbose();
    let db = new sqlite3.Database(this.dbName);

    let queryProm = new Promise<Meme[]>((resolve, reject) => {
      db.all(`SELECT id, name, url, top_prices.price FROM meme, (
                SELECT price.meme_id, price.price FROM price, (
                  SELECT meme_id, MAX(date) latest_date FROM price GROUP BY meme_id
                ) AS latest_entries
                WHERE price.meme_id = latest_entries.meme_id AND price.date = latest_entries.latest_date
                ORDER BY price.price DESC LIMIT 3
              ) AS top_prices
              WHERE meme.id = top_prices.meme_id
              ORDER BY top_prices.price DESC;`, [], (err, rows) => {
        if (err)
          return reject(`${err.message} top 3 memes query failed`);

        let top: Meme[] = [];

        for (const {id, name, url, price} of rows) {
          top.push(new Meme(name, url , price, { id: id }));
        }

        resolve(top);
      });
    });

    db.close();

    return await queryProm;
  }

  /*
    Zwraca mema o podanym id lub null jeśli nie istnieje mem o takim id
  */
  public async getMeme(id: number): Promise<Meme | null> {
    sqlite3.verbose();
    let db = new sqlite3.Database(this.dbName);

    let queryProm = new Promise<Meme | null>((resolve, reject) => {
      db.get('SELECT * FROM meme WHERE id = (?);', id, (err, row) => {
        if (err)
          return reject(`${err.message}, Błąd w trakcie wyszukiwania mema`);

        if (!row)
          return resolve(null);

        db.all('SELECT meme_id, date, price FROM price WHERE meme_id = (?) ORDER BY date DESC;', id, (err, rows) => {
          if (err)
            return reject(`${err.message} Błąd w trakcie wyszukiwania cen mema`);

          let datedPrices = rows.map(row => new DatedPrice(row.price, new Date(row.date)));
          let foundMeme = new Meme(row.name, row.url, rows[0].price, { id: id, priceHistory: new PriceHistory(datedPrices), db: this });

          resolve(foundMeme);
        });
      })
    });

    let queriedMeme = await queryProm;
    
    db.close();

    return queriedMeme;
  }

  public async addMemePrice(id: number, price: DatedPrice, author: User) {
    sqlite3.verbose();
    let db = new sqlite3.Database(this.dbName);

    await new Promise((resolve, reject) => {
      db.run('INSERT INTO price (meme_id, date, price, author_id) VALUES (?, ?, ?, ?);',
              [id, price.dateTime, price.price, author.id], (err) => {
        if (err)
          return reject(err);

        resolve();
      });
    });

    db.close();
  }
}