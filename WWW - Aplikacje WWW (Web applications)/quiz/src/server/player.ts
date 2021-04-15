export class Player {
  private memberID: number;
  private memberUsername: string;
  private memberPassword: string;

  constructor(id: number, username: string, password: string) {
    this.memberID = id;
    this.memberUsername = username;
    this.memberPassword = password;
  }

  get id(): number {
    return this.memberID;
  }

  get username(): string {
    return this.memberUsername;
  }

  public validPassword(password: string): boolean {
    return this.memberPassword === password;
  }
}