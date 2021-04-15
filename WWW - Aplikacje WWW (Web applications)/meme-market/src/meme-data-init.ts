/*
  Skrypt uruchamiany w celu wypełnienia bazy danych domyślnymi memami.
*/

import { Meme, PriceHistory, DatedPrice } from "./meme-classes";
import { memeDB } from "./meme-db-instance";

let memeDir = '/local_memes/';

memeDB.initDB().then(() => {
  return memeDB.addUser('admin', 'admin');
}).then((admin) => {
  let meme1PriceHistory = new PriceHistory([new DatedPrice(15, new Date(2011, 2, 2)),
                                            new DatedPrice(100, new Date(1992, 11, 25)),
                                            new DatedPrice(3000, new Date(1989, 10, 15))]);

  let meme1 = new Meme('Cool dog', memeDir + '1.jpg', 2099,
                        { author: admin, priceHistory: meme1PriceHistory, id: 1 });

  let meme2 = new Meme('Monkey', memeDir + '2.PNG', 115, { author: admin, id: 2 });

  let meme3 = new Meme('Film meme', memeDir + '3.png', 24, { author: admin, id: 3 });

  let meme4PriceHistory = new PriceHistory([new DatedPrice(900, new Date(2020, 3, 1)),
                                            new DatedPrice(1200, new Date(2000, 10, 13)),
                                            new DatedPrice(34000, new Date(1994, 1, 19))]);

  let meme4 = new Meme('Truckers', memeDir + '4.jpg', 2020,
                        { author: admin, priceHistory: meme4PriceHistory, id: 4 });

  let meme5PriceHistory = new PriceHistory([new DatedPrice(28, new Date(2017, 4, 2)),
                                            new DatedPrice(12, new Date(2000, 1, 13)),
                                            new DatedPrice(9090, new Date(1998, 2, 20)),
                                            new DatedPrice(8000, new Date(1992, 4, 23))]);

  let meme5 = new Meme('Free GTA download from Epic Games Store', memeDir + '5.jpg', 5005,
                        { author: admin, priceHistory: meme5PriceHistory, id: 5 });

  let meme6PriceHistory = new PriceHistory([new DatedPrice(321, new Date(2017, 4, 2)),
                                            new DatedPrice(12, new Date(2000, 1, 13))]);

  let meme6 = new Meme('Jus vibin', memeDir + '7.JPG', 54321000,
                        { author: admin, priceHistory: meme6PriceHistory, id: 6 });

  let meme7PriceHistory = new PriceHistory([new DatedPrice(450, new Date(2017, 4, 2))]);

  let meme7 = new Meme('Miss those days', memeDir + '6.jpg', 450,
                        { author: admin, priceHistory: meme7PriceHistory, id: 7 });

  let meme8PriceHistory = new PriceHistory([new DatedPrice(3000000, new Date(2018, 4, 1)),
                                            new DatedPrice(12000000, new Date(2005, 8, 12)),
                                            new DatedPrice(9000000, new Date(2002, 2, 20)),
                                            new DatedPrice(7654321, new Date(1992, 4, 22)),
                                            new DatedPrice(5, new Date(1985, 5, 16))]);

  let meme8 = new Meme('Nice', memeDir + '8.jpg', 20,
                        { author: admin, priceHistory: meme8PriceHistory, id: 8 });

  let meme9 = new Meme('Pou', memeDir + '9.jpg', 1234, { author: admin, id: 9 });

  let meme10PriceHistory = new PriceHistory([new DatedPrice(33, new Date(2020, 5, 17)),
                                              new DatedPrice(12, new Date(2010, 7, 20)),
                                              new DatedPrice(9090, new Date(1997, 9, 8)),
                                              new DatedPrice(8000, new Date(1995, 3, 2))]);

  let meme10 = new Meme('Facts', memeDir + '10.jpg', 77, { author: admin, priceHistory: meme10PriceHistory, id: 10 });

  memeDB.addMemes([meme1,
                   meme2,
                   meme3,
                   meme4,
                   meme5,
                   meme6,
                   meme7,
                   meme8,
                   meme9,
                   meme10], admin);
}).catch((err) => {
  console.log(err);
});