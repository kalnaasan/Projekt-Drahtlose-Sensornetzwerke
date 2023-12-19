import {Sensor} from "./sensor";

export class Room {

  public id!:number;
  public name!:string;
  public sensors:Sensor[] = [];
}
