import {Sensor} from "./sensor";

export class Room {

  public id!:string;
  public name!:string;
  public sensors:Sensor[] = [];
}
